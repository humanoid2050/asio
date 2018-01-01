#ifndef ASIO_CONNECTION_HPP
#define ASIO_CONNECTION_HPP

#include <deque>
#include <mutex>
#include <boost/signals2.hpp>
#include <functional>

#include "asio/asio_device.hpp"

using message = std::pair<int,std::shared_ptr<std::vector<uint8_t>>>;


class asio_connection : public asio_device
{
public:
//inherited API
    //std::shared_ptr<deviceDescription> get_description()
    //virtual bool stop()
    //virtual bool restart()
    
//inherited API overrides
    virtual bool start()
    {
        if (running_.test_and_set(std::memory_order_acquire)) return false;
        bool rc = connect();
        if (!rc) running_.clear(std::memory_order_release);
        return rc;
    }
    
    
//added API for connections
    virtual bool connect() { return false; };
    
    virtual bool disconnect() { return false; };
    
    bool send(message msg)
    {
        {
            std::lock_guard<std::mutex> lock(outbound_mutex_);
            outbound_msg_.push_back(msg);
            if (outbound_msg_.size() > max_out_que_)
            {
                outbound_msg_.pop_front();
            }
        }
        start_send();
        return true;
    }
    
    

//internal methods
    bool start_io()
    {
        start_send();
        start_receive();
    }
    
    bool start_send()
    {
        if (sending_.test_and_set(std::memory_order_acquire)) return false;
        return do_send();
    }
    
    virtual bool do_send() {};
    
    void handle_send(message msg, const boost::system::error_code& err, size_t count)
    {
        if (!err) {
            do_send();
        } else {
            restart();
        }
    }
    
    virtual void start_receive() {};
    
    void bind_on_start(std::function<bool (bool)> handler)
    {
        on_start.connect(handler);
    }
    
    void bind_on_stop(std::function<bool (bool)> handler)
    {
        on_stop.connect(handler);
    }
    
    void bind_notify(std::function<bool (int, std::string)> handler)
    {
        notify.connect(handler);
    }
    
    void bind_on_connect(std::function<bool (bool)> handler)
    {
        on_connect.connect(handler);
    }
    
    void bind_on_disconnect(std::function<bool (bool)> handler)
    {
        on_disconnect.connect(handler);
    }
    
    void bind_on_send(std::function<bool (uint32_t, bool)> handler)
    {
        on_send.connect(handler);
    }
    
    void bind_on_receive(std::function<bool (boost::asio::streambuf& msg_stream)> handler)
    {
        on_receive.connect(handler);
    }
    
protected:
    
    asio_connection(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_device(io_service, std::move(description)), connected_(ATOMIC_FLAG_INIT), connecting_(ATOMIC_FLAG_INIT)
    {
        
    }
    
    std::atomic_flag connecting_;
    std::atomic_flag connected_;
    
    std::mutex outbound_mutex_;
    std::deque<message> outbound_msg_;
    std::size_t max_out_que_;
    
    boost::signals2::signal<bool (bool)> on_start;
    boost::signals2::signal<bool (bool)> on_stop;
    boost::signals2::signal<bool (int, std::string)> notify;
    
    boost::signals2::signal<bool (bool)> on_connect;
    boost::signals2::signal<bool (bool)> on_disconnect;
    boost::signals2::signal<bool (uint32_t, bool)> on_send;
    boost::signals2::signal<bool (boost::asio::streambuf& msg_stream)> on_receive;

    std::atomic_flag sending_;
    
    boost::asio::streambuf buff_;
    
};


#endif
