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
    connection_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description) 
    {
        //device_ = deviceFactory::make_connection(io_service, std::move(description));
    }
    
    virtual bool on_connect(bool success = true) {};
    virtual bool on_disconnect(bool success = true) {};
    
    virtual bool on_send(uint32_t id = 0, bool success = true) {};
    virtual bool on_receive(uint32_t id = 0, bool success = true) {};
    
    //called directly, or chunked out 
    virtual bool handle_receive(boost::asio::streambuf& msg_stream)
    {
        
    }
    
protected:
    std::shared_ptr<asio_connection> device_;
    
    
};

#endif
