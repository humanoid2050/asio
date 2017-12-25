#ifndef ASIO_SERVER_HANDLER_HPP
#define ASIO_SERVER_HANDLER_HPP

#include "asio/connection_handler.hpp"
#include "asio/tcp_client_handler.hpp"
#include "asio/device_description.hpp"
#include "asio/tcp_server.hpp"

class tcp_server;

class tcp_server_handler : public connection_handler
{
public:
    tcp_server_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description);
    
    virtual std::shared_ptr<tcp_client_handler> get_connection_handler();

private:
    std::unique_ptr<tcp_server> device_;
    
    std::vector<std::shared_ptr<tcp_client_handler>> connections_;
    
    boost::asio::io_service& io_srv_;
};

#endif
