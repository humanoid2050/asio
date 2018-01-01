#ifndef ASIO_DEVICE_HPP
#define ASIO_DEVICE_HPP

#include <memory>
#include <atomic>
#include <functional> 

#include <boost/asio.hpp>

#include "asio/device_description.hpp"
//#include "asio/handler_base.hpp"


class asio_device
{
public:
    asio_device(boost::asio::io_service & service, std::unique_ptr<deviceDescription> description)
        : description_(std::move(description)), running_(ATOMIC_FLAG_INIT)
    {
    }
    
    std::shared_ptr<deviceDescription> get_description()
    {
        return std::make_shared<deviceDescription>();
    }
    
   
    virtual bool start() { return false; };
    
    virtual bool stop() { return true; };
    
    virtual bool restart()
    {
        stop();
        if (description_->is_persistent()) start();
        return true;
    };
    
protected:

    std::shared_ptr<deviceDescription> description_; 
    
    std::atomic_flag running_;
    
    
    
};


#endif
