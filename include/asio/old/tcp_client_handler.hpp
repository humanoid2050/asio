#ifndef ASIO_TCP_CLIENT_HANDLER_HPP
#define ASIO_TCP_CLIENT_HANDLER_HPP

#include "asio/connection_handler.hpp"
#include "asio/tcp_client.hpp"

class tcp_client;

class tcp_client_handler : public connection_handler
{
public:
    tcp_client_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description = std::unique_ptr<deviceDescription>());
    
    void kickstart(const boost::system::error_code& err);
        
    boost::asio::ip::tcp::socket& get_socket();
    
    //useufl if underlying connection uses the asio_read, asio_read_at, or
    //asio_read_until free functions returning stream_buf types
    virtual bool handle_receive(std::streambuf& msg_stream);

private:
    std::unique_ptr<tcp_client> device_;
};

#endif
