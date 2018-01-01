#ifndef ASIO_DGRAM_CONNECTION_HPP
#define ASIO_DGRAM_CONNECTION_HPP

#include "asio/asio_connection.hpp"
#include "asio/socket_description.hpp"

#include <mutex>
#include <deque>

#include "boost/bind.hpp"


template <class conn_T>
class dgram_connection : public asio_connection
{
public:
    
    bool do_send()
    {
        message msg;
        //critial operations are the check, copy, and pop
        {
            std::lock_guard<std::mutex> lock(outbound_mutex_);
            if (outbound_msg_.empty())
            {
                sending_.clear(std::memory_order_release);
                return false;
            }
            msg = outbound_msg_.front();
            outbound_msg_.pop_front();
        }
        socket_.async_send(boost::asio::buffer(*msg.second), boost::bind(&dgram_connection<conn_T>::handle_send, this, msg,
                                 boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        return true;
    }
 
    void start_receive(std::size_t count = 1024*16)
    {
        auto buff = buff_.prepare(count);
        socket_.async_receive(buff, boost::bind(&dgram_connection<conn_T>::handle_receive, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& err, std::size_t count)
    {
        if (!err)
        {
            on_receive(buff_);
            //the handler should queue up the next receive
            start_receive();
        }
        else
        {
            restart();
        }
    }

    conn_T& get_socket()
    {
        return socket_;
    }

protected:
    dgram_connection(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description)), socket_(io_service)
    {
        
    }

    conn_T socket_;

};



class udp_client : public dgram_connection<boost::asio::ip::udp::socket> 
{
public:
    udp_client(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : dgram_connection<boost::asio::ip::udp::socket>(io_service, std::move(description)), resolver_(io_service)
    {
        
    }
    
    bool connect()
    {
        if (connected_.test_and_set(std::memory_order_acquire)) return false;
        auto desc = std::static_pointer_cast<socketDescription>(description_);
        boost::asio::ip::udp::resolver::query query(desc->get_address(), std::to_string(desc->get_port()));
        resolver_.async_resolve(query, boost::bind(&udp_client::handle_resolve, this, 
                                boost::asio::placeholders::error, boost::asio::placeholders::iterator));
        return true;
    }

    void handle_resolve(const boost::system::error_code& err, boost::asio::ip::udp::resolver::iterator endpoint_iterator)
    {
        if (!err)
        {
            // Attempt a connection to each endpoint in the list until we
            // successfully establish a connection.
            boost::asio::async_connect(socket_, endpoint_iterator, boost::bind(&udp_client::handle_connect, this, boost::asio::placeholders::error));
        }
        else if (err != boost::asio::error::operation_aborted)
        {
            restart();
        }
    }

    void handle_connect(const boost::system::error_code& err)
    {
        if (!err)
        {
            connected_.test_and_set(std::memory_order_acquire);
            on_connect(true);
            start_io();
        }
        else if (err != boost::asio::error::operation_aborted)
        {
            restart();
        }
    }
    
    virtual bool stop()
    {
        resolver_.cancel();
        socket_.shutdown(boost::asio::ip::udp::socket::shutdown_both);
        socket_.cancel();
        socket_.close();
        connected_.clear(std::memory_order_release);
        on_disconnect(true);
        running_.clear(std::memory_order_release);
    }

protected:
    boost::asio::ip::udp::resolver resolver_;
};



class udp_server : public dgram_connection<boost::asio::ip::udp::socket> 
{
public:
    udp_server(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : dgram_connection<boost::asio::ip::udp::socket>(io_service, std::move(description)), resolver_(io_service)
    {
        
    }
    
    bool connect()
    {
        if (connected_.test_and_set(std::memory_order_acquire)) return false;
        auto desc = std::static_pointer_cast<socketDescription>(description_);
        
        boost::asio::ip::udp::endpoint ep(boost::asio::ip::udp::v4(), desc->get_port());
        if (!desc->get_address().empty())
            ep.address(boost::asio::ip::address::from_string(desc->get_address()));
        
        socket_.open(boost::asio::ip::udp::v4());
        boost::system::error_code ec;
        socket_.bind(ep,ec);
        handle_connect (ec);
        
        return true;
    }


    void handle_connect(const boost::system::error_code& err)
    {
        if (!err)
        {
            connected_.test_and_set(std::memory_order_acquire);
            on_connect(true);
            start_io();
        }
        else if (err != boost::asio::error::operation_aborted)
        {
            restart();
        }
    }
    
    virtual bool stop()
    {
        resolver_.cancel();
        socket_.shutdown(boost::asio::ip::udp::socket::shutdown_both);
        socket_.cancel();
        socket_.close();
        connected_.clear(std::memory_order_release);
        on_disconnect(true);
        running_.clear(std::memory_order_release);
    }

protected:
    boost::asio::ip::udp::resolver resolver_;
};



class local_dgram_client : public dgram_connection<boost::asio::local::datagram_protocol::socket>
{
public:
    local_dgram_client(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : dgram_connection<boost::asio::local::datagram_protocol::socket>(io_service, std::move(description))
    {
        
    }
    
    virtual bool connect()
    {
        auto desc = std::static_pointer_cast<socketDescription>(description_);
        boost::asio::local::datagram_protocol::endpoint ep(desc->get_address());
        socket_.async_connect(ep, boost::bind(&local_dgram_client::handle_connect, this, boost::asio::placeholders::error));
    }
    
    void handle_connect(const boost::system::error_code& err)
    {
        if (!err)
        {
            connected_.test_and_set(std::memory_order_acquire);
            on_connect(true);
            start_io();
        }
        else if (err != boost::asio::error::operation_aborted)
        {
            restart();
        }
    }
    
    virtual bool stop()
    {
        socket_.shutdown(boost::asio::ip::udp::socket::shutdown_both);
        socket_.cancel();
        socket_.close();
        connected_.clear(std::memory_order_release);
        on_disconnect(true);
        running_.clear(std::memory_order_release);
    }
};


class local_dgram_server : public dgram_connection<boost::asio::local::datagram_protocol::socket>
{
public:
    local_dgram_server(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : dgram_connection<boost::asio::local::datagram_protocol::socket>(io_service, std::move(description))
    {
        
    }
    
    virtual bool connect()
    {
        auto desc = std::static_pointer_cast<socketDescription>(description_);
        
        socket_.open(boost::asio::local::datagram_protocol());
        boost::asio::local::datagram_protocol::endpoint ep(desc->get_address());
        boost::system::error_code ec;
        socket_.bind(ep,ec);
        handle_connect(ec);
    }
    
    void handle_connect(const boost::system::error_code& err)
    {
        if (!err)
        {
            connected_.test_and_set(std::memory_order_acquire);
            on_connect(true);
            start_io();
        }
        else if (err != boost::asio::error::operation_aborted)
        {
            restart();
        }
    }
    
    virtual bool stop()
    {
        socket_.shutdown(boost::asio::ip::udp::socket::shutdown_both);
        socket_.cancel();
        socket_.close();
        connected_.clear(std::memory_order_release);
        on_disconnect(true);
        running_.clear(std::memory_order_release);
    }
};

#endif
