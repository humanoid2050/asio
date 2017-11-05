#ifndef ASIO_TCP_SERVER_HPP
#define ASIO_TCP_SERVER_HPP

#include "asio/connection.hpp"
#include "asio/server_handler.hpp"

using boost::asio::ip::tcp;

//open specified socket
//wait for connection
//create connection object
//register and track connection until disconnect
//do_send: pass data to all connection objects

class tcp_server : public asio_connection
{
public:
    tcp_server(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description)), acceptor_(io_service,tcp::v4())
    {
        
    }
    
    bool start()
    {
        if (running_.test_and_set(std::memory_order_acquire)) return false;
        tcp::endpoint ep(tcp::v4(), std::static_pointer_cast<socketDescription>(description_)->get_port());
        acceptor_.bind(ep);
        acceptor_.listen();
        
        //server handler is responsible for providing a pointer to an object which will provide the connection API
        //this may be the server handler itself, or a new object specificially created for that new connection
        if (auto h = handler_.lock()) 
        {
            std::shared_ptr<connection_handler> sh = std::static_pointer_cast<server_handler>(h)->get_connection_handler();
            acceptor_.async_accept(std::static_pointer_cast<tcp_client>(sh->get_device())->get_socket(), boost::bind(&tcp_server::handle_accept,this,boost::asio::placeholders::error));
            return true;
        }
        return false;
    }
    
    void handle_accept(const boost::system::error_code& error)
    {
        //bind handler back to metahandler (may be a useless connection)
    }
    
    
    
    
    tcp::acceptor acceptor_;
    
};

#endif
