#include "asio/serial_handler.hpp"


serial_handler::serial_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
{
    device_ =  std::unique_ptr<asio_serial>(new asio_serial(service, std::move(description)));
}


boost::asio::serial_port& serial_handler::get_socket()
{
    return device_->socket();
}

//useufl if underlying connection uses the asio_read, asio_read_at, or
//asio_read_until free functions returning stream_buf types
bool serial_handler::handle_receive(std::streambuf& msg_stream)
{
    
}
