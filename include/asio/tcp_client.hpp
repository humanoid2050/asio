#ifndef ASIO_TCP_CLIENT_HPP
#define ASIO_TCP_CLIENT_HPP

#include <deque>
#include <mutex>
#include <sstream>
#include "asio/connection.hpp"
#include "asio/stream_handler.hpp"
#include "boost/bind.hpp"

using boost::asio::ip::tcp;

class tcp_client : public asio_connection
{
public:
    tcp_client(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description)), sending_(ATOMIC_FLAG_INIT), resolver_(io_service), socket_(io_service)
    {
        
    }
    
    tcp::socket& socket()
    {
        return socket_;
    }
    
    virtual bool start()
    {
        if (running_.test_and_set(std::memory_order_acquire)) return false;
        auto desc = std::static_pointer_cast<socketDescription>(description_);
        tcp::resolver::query query(desc->get_host(), std::to_string(desc->get_port()));
        resolver_.async_resolve(query, boost::bind(&tcp_client::handle_resolve, this, 
                                boost::asio::placeholders::error, boost::asio::placeholders::iterator));
        return true;
    }
    
    void handle_resolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator)
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
    
    void handle_connect(const boost::system::error_code& err)
    {
        if (!err)
        {
            if (auto h = handler_.lock()) std::static_pointer_cast<connection_handler>(h)->on_connect();
            start_send();
            start_receive();
        }
        else if (err != boost::asio::error::operation_aborted)
        {
            restart();
        }
    }
    
    virtual bool send(std::shared_ptr<std::stringstream> msg)
    {
        //critical operation is just the push
        {
            std::lock_guard<std::mutex> lock(outbound_mutex_);
            outbound_messages_.push_back(msg);
        }
        start_send();
        return true;
    }
    
    bool start_send()
    {
        if (sending_.test_and_set(std::memory_order_acquire)) return false;
        do_send();
        return true;
    }
    
    bool do_send()
    {
        std::shared_ptr<std::stringstream> msg;
        //critial operations are the check, copy, and pop
        {
            std::lock_guard<std::mutex> lock(outbound_mutex_);
            if (outbound_messages_.empty())
            {
                sending_.clear(std::memory_order_release);
                return false;
            }
            msg = outbound_messages_.front();
            outbound_messages_.pop_front();
        }
        //boost::asio::async_write(socket_, *msg, boost::bind(&tcp_client::handle_send, this, msg,
        //                         boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        return true;
    }
    
    void handle_send(std::shared_ptr<std::stringstream> msg, const boost::system::error_code& err, size_t count)
    {
        if (!err)
        {
            do_send();
        }
        else
        {
            restart();
        }
        
    }
    
    void start_receive()
    {
        boost::asio::async_read(socket_, buff_, boost::asio::transfer_at_least(1), boost::bind(&tcp_client::handle_receive, this, 
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
    
    void handle_receive(const boost::system::error_code& err, std::size_t count)
    {
        if (!err)
        {
            if (auto h = handler_.lock()) std::static_pointer_cast<stream_handler>(h)->handle_receive(buff_);
            start_receive();
        }
        else
        {
            restart();
        }
    }
    
    virtual bool stop()
    {
        
        //resolver_.cancel();
        //socket_.shutdown(tcp::socket::shutdown_both);
        //socket_.close();
        if (auto h = handler_.lock()) std::static_pointer_cast<connection_handler>(h)->on_disconnect();
        running_.clear(std::memory_order_release);
    }
    
    virtual bool restart()
    {
        stop();
        start();
    }
    
    tcp::socket & get_socket()
    {
        return socket_;
    }
    
protected:

    tcp::resolver resolver_;
    tcp::socket socket_;
    
    boost::asio::streambuf buff_;
    
    
    std::mutex outbound_mutex_;
    std::deque<std::shared_ptr<std::stringstream>> outbound_messages_;
    
    std::atomic_flag sending_;
    
};

#endif
