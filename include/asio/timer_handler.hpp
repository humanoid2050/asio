#ifndef ASIO_TIMER_HANDLER_HPP
#define ASIO_TIMER_HANDLER_HPP

#include "asio/handler_base.hpp"

class timer_handler : public handler_base
{
public:
    timer_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
    {
        device_ = deviceFactory::make_timer(io_service, std::move(description));
    }
    
    virtual bool on_timeout(bool success);
    
protected:
    std::shared_ptr<asio_timer> device_;
}

#endif
