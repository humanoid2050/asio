#include "asio/tcp_client_handler.hpp"


tcp_client_handler::tcp_client_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
{
    device_ =  std::unique_ptr<tcp_client>(new tcp_client(io_service, std::move(description)));
}

void tcp_client_handler::kickstart(const boost::system::error_code& err)
{
    device_->handle_connect(err);
}

boost::asio::ip::tcp::socket& tcp_client_handler::get_socket()
{
    return device_->socket();
}

//useufl if underlying connection uses the asio_read, asio_read_at, or
//asio_read_until free functions returning stream_buf types
bool tcp_client_handler::handle_receive(std::streambuf& msg_stream)
{
    
}
