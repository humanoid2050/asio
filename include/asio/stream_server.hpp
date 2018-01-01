#ifndef ASIO_STREAM_SERVER_HPP
#define ASIO_STREAM_SERVER_HPP

#include <list>

#include "asio/asio_connection.hpp"
#include "asio/connection_handler.hpp"
#include "asio/stream_connection.hpp"

class connection_handler;

//this variant is specifically for single/equivalent connections where all socket objects are bound 
//back to the same handler conforming to the connection_handler API
template<class family_T>
class singular_stream_server : public asio_connection
{
public:
// inherited API
    //std::shared_ptr<deviceDescription> get_description()
    //virtual bool start()
    //virtual bool stop()
    //virtual bool restart()
    //virtual bool connect() { return false; };
    //virtual bool disconnect() { return false; };
    //bool send(message msg)
    

    bool stop()
    {
        acceptor_.cancel();
        acceptor_.close();
        for (auto itr = clients_.begin(); itr != clients_.end(); ++itr)
        {
            (*itr)->stop();
        }
        clients_.clear();
        running_.clear(std::memory_order_release);
    }

    bool send(message msg)
    {
        for (auto itr = clients_.begin(); itr != clients_.end(); ++itr)
        {
            (*itr)->send(msg);
        }
    }
    
protected:    
    singular_stream_server(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description, std::shared_ptr<connection_handler> handler, std::size_t max_connection_count = 1)
        : asio_connection(io_service, std::move(description), std::move(handler)), acceptor_(io_service), max_connection_count_(max_connection_count)
    {
        
    }
    
    bool start_accept()
    {
        if (accepting_.test_and_set(std::memory_order_acquire)) return false;
        if (auto handler = handler_.lock())
        {
            
            //create connection object
            auto parent_desc = static_cast<socketDescription*>(description_.get());
            auto client_desc = std::unique_ptr<socketDescription>(new socketDescription(parent_desc->get_domain(),parent_desc->get_protocol()));
            //create the connection prebound to the handler that owns this stream_server object
            clients_.emplace_back(new stream_connection<typename family_T::socket>(acceptor_.get_io_service(), std::move(client_desc), std::move(handler)));
            
            acceptor_.async_accept(clients_.back()->get_socket(),    
                boost::bind(&singular_stream_server<family_T>::handle_accept,this, boost::asio::placeholders::error));
            return true;
        }
        accepting_.clear(std::memory_order_release);
        return false;
        
    }

    void handle_accept(const boost::system::error_code& err)
    {
        accepting_.clear(std::memory_order_release);
        if (!err)
        {
            //if no errors, kickstart the connection
            //clients_.push_back(std::move(conn));
            clients_.back()->handle_connect(err);
            if (clients_.size() < max_connection_count_)
            {
                //go back into an accepting state if more connections can be accepted
                start_accept();
            }
        }
        if (err != boost::asio::error::operation_aborted)
        {
            //if there was an error (other than user killing the connection), go back to an accepting state
            //implicitlly, the connection leaves scope and the previous connection object is dumped.
            start_accept();
        }
        
    }
    
    void validate_clients()
    {
        clients_.remove_if([](std::unique_ptr<stream_connection<typename family_T::socket>> cli){cli.is_disconnected();});
        if (clients_.size() < max_connection_count_)
        {
            //go back into an accepting state if more connections can be accepted
            start_accept();
        }
    }
    
    std::atomic_flag accepting_;
    boost::asio::basic_socket_acceptor<family_T> acceptor_;
    std::size_t max_connection_count_;
    
    std::list<std::unique_ptr<stream_connection<typename family_T::socket>>> clients_;
};


class tcp_server : public singular_stream_server<boost::asio::ip::tcp>
{
public:
    tcp_server(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description, std::shared_ptr<connection_handler> handler)
        : singular_stream_server<boost::asio::ip::tcp>(io_service, std::move(description), std::move(handler))
    {
        
    }

protected:
    bool connect()
    {
        auto desc = std::static_pointer_cast<socketDescription>(description_);
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), desc->get_port());
        if (!desc->get_address().empty())
            ep.address(boost::asio::ip::address::from_string(desc->get_address()));
        
        acceptor_.open(boost::asio::ip::tcp::v4());
        acceptor_.bind(ep);
        acceptor_.listen();
        
        //server handler is responsible for providing a pointer to an object which will provide the connection API
        //this may be the server handler itself, or a new object specificially created for that new connection
        return start_accept();
        
    }
};

class local_stream_server : public singular_stream_server<boost::asio::local::stream_protocol>
{
public:
    local_stream_server(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description, std::shared_ptr<connection_handler> handler)
        : singular_stream_server<boost::asio::local::stream_protocol>(io_service, std::move(description), std::move(handler))
    {
        
    }
    
protected:
    bool connect()
    {
        boost::asio::local::stream_protocol::endpoint ep(std::static_pointer_cast<socketDescription>(description_)->get_address());
        acceptor_.open();
        acceptor_.bind(ep);
        acceptor_.listen();
        
        //server handler is responsible for providing a pointer to an object which will provide the connection API
        //this may be the server handler itself, or a new object specificially created for that new connection
        return start_accept();
        
    }
};


#endif
