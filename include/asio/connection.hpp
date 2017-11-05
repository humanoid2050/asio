#ifndef ASIO_CONNECTION_HPP
#define ASIO_CONNECTION_HPP

#include "asio/device.hpp"
//#include "asio/connection_handler.hpp"

class asio_connection : public asio_device
{
public:
    asio_connection(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_device(io_service, std::move(description))
    {
        
    }
    
    virtual void connect() {};
    
    virtual void disconnect() {};
    
    virtual void send() {};

protected:
    std::atomic_flag connected_;
    
};


#endif
