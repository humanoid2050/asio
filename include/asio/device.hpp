#ifndef ASIO_DEVICE_HPP
#define ASIO_DEVICE_HPP

#include <memory>
#include <atomic>
#include <functional> 

#include <boost/asio.hpp>

#include "asio/device_description.hpp"
#include "asio/handler_base.hpp"


class asio_device
{
public:
    asio_device(boost::asio::io_service & service, std::unique_ptr<deviceDescription> description = std::unique_ptr<deviceDescription>())
        : description_(std::move(description)), running_(ATOMIC_FLAG_INIT)
    {
    }
    
    std::shared_ptr<deviceDescription> get_description()
    {
        return std::make_shared<deviceDescription>();
    }
    
   
    virtual bool start() {};
    
    virtual bool stop() {};
    
    virtual bool restart() {};
    
public:

    std::weak_ptr<handler_base> handler_;
    std::shared_ptr<deviceDescription> description_; 
    
    std::atomic_flag running_;
    
    
    
};


#endif
