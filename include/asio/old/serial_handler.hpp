#ifndef ASIO_SOCKET_HANDLER_HPP
#define ASIO_SOCKET_HANDLER_HPP


#include "asio/connection_handler.hpp"
#include "asio/serial.hpp"

class asio_serial;

class serial_handler : public connection_handler
{
public:
    serial_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description = std::unique_ptr<deviceDescription>());
        
    boost::asio::serial_port& get_socket();
    
    //useufl if underlying connection uses the asio_read, asio_read_at, or
    //asio_read_until free functions returning stream_buf types
    virtual bool handle_receive(std::streambuf& msg_stream);

private:
    std::unique_ptr<asio_serial> device_;
};


#endif
