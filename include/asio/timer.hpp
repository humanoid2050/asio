#ifndef ASIO_TIMER_HPP
#define ASIO_TIMER_HPP

#include <boost/asio/steady_timer.hpp>
#include "asio/device.hpp"
#include "timer_description.hpp"


class asio_timer : public asio_device
{
public:
    asio_timer(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_device(io_service, std::move(description)), timer_(io_service)
    {
        std::static_pointer_cast<timerDescription>(description_)->get_interval();
    }
    
    virtual bool start()
    {
        //timer_.expires_from_now(std::chrono::seconds(60));
    }
    
    
    
protected:
    
    boost::asio::steady_timer timer_;
    
    std::function<bool()> on_expire_;
    
};

#endif
