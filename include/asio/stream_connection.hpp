#ifndef ASIO_STREAM_CONNECTION_HPP
#define ASIO_STREAM_CONNECTION_HPP

#include "asio/asio_connection.hpp"
#include "asio/connection_handler.hpp"
#include "asio/socket_description.hpp"
#include "asio/serial_description.hpp"

#include <mutex>
#include <deque>

#include "boost/bind.hpp"

class connection_handler;

template <class conn_T>
class stream_connection_base : public asio_connection
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
    
    bool connect() { return false; };
    bool stop() { return false; };
    
    stream_connection_base(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description)), socket_(io_service)
    {
        
    }
//override internal methods
    void handle_connect(const boost::system::error_code& err)
    {
        if (connected_.test_and_set(std::memory_order_acquire)) return;
        if (!err)
        {
            on_connect(true);
            start_io();
        }
        else if (err != boost::asio::error::operation_aborted)
        {
            restart();
        }
    }
    
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
        boost::asio::async_write(socket_, boost::asio::buffer(*msg.second), boost::bind(&stream_connection_base<conn_T>::handle_send, this, msg,
                                 boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        return true;
    }
    

    void start_receive()
    {
        boost::asio::async_read(socket_, buff_, boost::asio::transfer_at_least(1), boost::bind(&stream_connection_base<conn_T>::handle_receive, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    
    void start_receive(std::size_t count)
    {
        boost::asio::async_read(socket_, buff_, boost::asio::transfer_exactly(count), boost::bind(&stream_connection_base<conn_T>::handle_receive, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    
    void start_receive(std::string delim)
    {
        boost::asio::async_read_until(socket_, buff_, delim, boost::bind(&stream_connection_base<conn_T>::handle_receive, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }


    void handle_receive(const boost::system::error_code& err, std::size_t count)
    {
        if (!err)
        {
            on_receive(buff_);
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
    conn_T socket_;
    
    boost::asio::streambuf buff_;

};

template<class conn_T>
class stream_connection : stream_connection_base<conn_T>
{
    stream_connection(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : stream_connection_base<boost::asio::ip::tcp::socket>(io_service, std::move(description))
    {
        
    }
    
};



typedef stream_connection<boost::asio::ip::tcp::socket> tcp_client;

template<>
class stream_connection<boost::asio::ip::tcp::socket> : public stream_connection_base<boost::asio::ip::tcp::socket> 
{
public:
    stream_connection(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : stream_connection_base<boost::asio::ip::tcp::socket>(io_service, std::move(description)), resolver_(io_service)
    {
        
    }
    
    bool connect()
    {
        if (connecting_.test_and_set(std::memory_order_acquire)) return false;
        auto desc = std::static_pointer_cast<socketDescription>(description_);
        boost::asio::ip::tcp::resolver::query query(desc->get_address(), std::to_string(desc->get_port()));
        resolver_.async_resolve(query, boost::bind(&tcp_client::handle_resolve, this, 
                                boost::asio::placeholders::error, boost::asio::placeholders::iterator));
        return true;
    }
    
    
    bool stop()
    {
        resolver_.cancel();
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket_.cancel();
        socket_.close();
        //only perform on_disconnect if the connected flag has previously been thrown
        if (connected_.test_and_set(std::memory_order_acquire))
        {
            on_disconnect(true);
        }
        connected_.clear(std::memory_order_release);
        //putting the connecting_ flag release after the connected_ checks prevents race conditions
        connecting_.clear(std::memory_order_release);
        running_.clear(std::memory_order_release);
    }

protected:

    void handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
    {
        if (!err)
        {
            // Attempt a connection to each endpoint in the list until we
            // successfully establish a connection.
            boost::asio::async_connect(socket_, endpoint_iterator, boost::bind(&tcp_client::handle_connect, this, boost::asio::placeholders::error));
        }
        else if (err != boost::asio::error::operation_aborted)
        {
            restart();
        }
    }


    boost::asio::ip::tcp::resolver resolver_;
};



typedef stream_connection<boost::asio::local::stream_protocol::socket> local_stream_client;

template<>
class stream_connection<boost::asio::local::stream_protocol::socket> : public stream_connection_base<boost::asio::local::stream_protocol::socket>
{
public:
    stream_connection(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : stream_connection_base<boost::asio::local::stream_protocol::socket>(io_service, std::move(description))
    {
        
    }
    
    bool connect()
    {
        if (connecting_.test_and_set(std::memory_order_acquire)) return false;
        auto desc = std::static_pointer_cast<socketDescription>(description_);
        boost::asio::local::stream_protocol::endpoint ep(desc->get_address());
        socket_.async_connect(ep, boost::bind(&local_stream_client::handle_connect, this, boost::asio::placeholders::error));
    }
    
    bool stop()
    {
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket_.cancel();
        socket_.close();
        //only perform on_disconnect if the connected flag has previously been thrown
        if (connected_.test_and_set(std::memory_order_acquire))
        {
            on_disconnect(true);
        }
        connected_.clear(std::memory_order_release);
        //putting the connecting_ flag release after the connected_ checks prevents race conditions
        connecting_.clear(std::memory_order_release);
        running_.clear(std::memory_order_release);
    }
};



typedef stream_connection<boost::asio::serial_port> serial_device;

template<>
class stream_connection<boost::asio::serial_port> : public stream_connection_base<boost::asio::serial_port>
{
public:
    stream_connection(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : stream_connection_base<boost::asio::serial_port>(io_service, std::move(description))
    {
        
    }
    
    bool connect()
    {
        if (connecting_.test_and_set(std::memory_order_acquire)) return false;
        auto desc = std::static_pointer_cast<serialDescription>(description_);
        try 
        {
            notify(1,std::string("connecting"));
            socket_.open(desc->get_device());
            socket_.set_option(boost::asio::serial_port::baud_rate(static_cast<uint32_t>(desc->get_baud())));
        } catch (boost::system::system_error& se) {
            socket_.close();
            connected_.clear(std::memory_order_release);
            return false;
        }
        //socket_.set_option();
        handle_connect(boost::system::error_code());
    }
    
    bool stop()
    {
        socket_.cancel();
        socket_.close();
        //only perform on_disconnect if the connected flag has previously been thrown
        if (connected_.test_and_set(std::memory_order_acquire))
        {
            on_disconnect(true);
        }
        connected_.clear(std::memory_order_release);
        //putting the connecting_ flag release after the connected_ checks prevents race conditions
        connecting_.clear(std::memory_order_release);
        running_.clear(std::memory_order_release);
    }
};



#endif
