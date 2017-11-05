#ifndef ASIO_TIMER_HANDLER_HPP
#define ASIO_TIMER_HANDLER_HPP

#include "asio/handler_base.hpp"

class timer_handler : public handler_base
{
public:
    timer_handler();
    
    virtual bool handle_timeout(bool success);
}

#endif
