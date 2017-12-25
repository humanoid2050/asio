#ifndef ASIO_CONNECTION_HPP
#define ASIO_CONNECTION_HPP

#include <deque>
#include <mutex>

#include "asio/device.hpp"
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
    
    void handle_send(std::shared_ptr<std::vector<uint8_t>> msg, const boost::system::error_code& err, size_t count)
    {
        if (!err) {
            do_send();
        } else {
            restart();
        }
    }
    
    virtual void start_receive() {};
    
    void bind_handler(std::shared_ptr<connection_handler> handler)
    {
        handler_ = handler;
    }

protected:
    
    asio_connection(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description, std::shared_ptr<connection_handler> handler)
        : asio_device(io_service, std::move(description)), connected_(ATOMIC_FLAG_INIT), handler_(handler)
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
