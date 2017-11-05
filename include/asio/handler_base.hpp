#ifndef ASIO_HANDLER_BASE_HPP
#define ASIO_HANDLER_BASE_HPP

#include "asio/device.hpp"

class asio_device;

class handler_base
{
public:
    
    handler_base()
    {
        
    }
    
    virtual void handle_start(bool success = true) {};
    virtual void handle_stop(bool success = true) {};
    virtual void handle_notify(int level, std::string message) {};
    
    virtual std::shared_ptr<asio_device> get_device()
    {
        return device_;
    }
    
protected:
    std::shared_ptr<asio_device> device_;
};

#endif
