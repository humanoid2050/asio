#ifndef ASIO_CONNECTION_HPP
#define ASIO_CONNECTION_HPP

#include <deque>
#include <mutex>

#include "asio/asio_device.hpp"
#include "asio/connection_handler.hpp"

using message = std::pair<int,std::shared_ptr<std::vector<uint8_t>>>;

class connection_handler;

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
        notify(log_level::DEEP_DEBUG,"asio_connection::start()");
        if (running_.test_and_set(std::memory_order_acquire)) return false;
        bool rc = connect();
        if (!rc) running_.clear(std::memory_order_release);
        return rc;
    }
    
    
//added API for connections
    virtual bool connect()
    {
        return false;
        notify(log_level::DEEP_DEBUG,"asio_connection::connect()");
    };
    
    virtual bool disconnect()
    {
        return false;
        notify(log_level::DEEP_DEBUG,"asio_connection::disconnect()");
    };
    
    bool send(message msg)
    {
        notify(log_level::DEEP_DEBUG,"asio_connection::send(message msg)");
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
        notify(log_level::DEEP_DEBUG,"asio_connection::start_io()");
        start_send();
        start_receive();
    }
    
    bool start_send()
    {
        notify(log_level::DEEP_DEBUG,"asio_connection::start_send()");
        if (sending_.test_and_set(std::memory_order_acquire)) return false;
        return do_send();
    }
    
    virtual bool do_send()
    {
        notify(log_level::DEEP_DEBUG,"asio_connection::do_send()");
        return false;
    };
    
    void handle_send(message msg, const boost::system::error_code& err, size_t count)
    {
        notify(log_level::DEEP_DEBUG,"asio_connection::handle_send()");
        if (!err) {
            do_send();
        } else {
            restart();
        }
    }
    
    virtual void start_receive()
    {
        notify(log_level::DEEP_DEBUG,"asio_connection::start_receive()");
    };
    
    
//some convenience methods
    void bind_handler(std::shared_ptr<connection_handler> handler)
    {
        handler_ = handler;
    }
    
    inline bool on_start(bool success = true)
    {
        if (auto h = handler_.lock()) return h->on_start(success);
        else return true;
    }
    
    inline bool on_stop(bool success = true)
    {
        if (auto h = handler_.lock()) return h->on_stop(success);
        else return true;
    }
    
    inline void notify(log_level level, std::string message)
    {
        if (auto h = handler_.lock()) h->notify(level,message);
    }
    
    inline bool on_connect(bool success = true)
    {
        if (auto h = handler_.lock()) return h->on_connect(success);
        else return true;
    }
    inline bool on_disconnect(bool success = true)
    {
        if (auto h = handler_.lock()) return h->on_disconnect(success);
        else return true;
    }
    
    inline bool on_send(uint32_t id = 0, bool success = true)
    {
        if (auto h = handler_.lock()) return h->on_send(id,success);
        else return true;
    }
    
    inline bool on_receive(boost::asio::streambuf& msg_stream)
    {
        if (auto h = handler_.lock()) return h->on_receive(msg_stream);
        else return true;
    }
    
    
protected:
    
    asio_connection(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description, std::shared_ptr<connection_handler> handler)
        : asio_device(io_service, std::move(description)), connected_(ATOMIC_FLAG_INIT), connecting_(ATOMIC_FLAG_INIT), handler_(handler)
    {
        
    }
    
    std::atomic_flag connecting_;
    std::atomic_flag connected_;
    
    std::mutex outbound_mutex_;
    std::deque<message> outbound_msg_;
    std::size_t max_out_que_;
    
    std::weak_ptr<connection_handler> handler_;
    
    std::atomic_flag sending_;
    
    boost::asio::streambuf buff_;
    
};


#endif
