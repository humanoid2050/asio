#ifndef ASIO_CONNECTION_HANDLER_HPP
#define ASIO_CONNECTION_HANDLER_HPP

#include "asio/handler_base.hpp"
#include "asio/asio_connection.hpp"
//#include "asio/factory.hpp"

class asio_connection;
//class deviceFactory;

class connection_handler : public handler_base
{
public:
    connection_handler() 
    {
        
    }
    
    virtual bool on_connect(bool success = true) {}
    virtual bool on_disconnect(bool success = true) {}
    
    virtual bool on_send(uint32_t id = 0, bool success = true) {}
    virtual bool on_receive(boost::asio::streambuf& msg_stream) {}
    
    
    
};

#endif
