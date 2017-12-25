#include "asio/tcp_server_handler.hpp"

tcp_server_handler::tcp_server_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
    : io_srv_(io_service)
{
    device_ =  std::unique_ptr<tcp_server>(new tcp_server(io_service, std::move(description)));
}

std::shared_ptr<tcp_client_handler> tcp_server_handler::get_connection_handler()
{
     connections_.emplace_back(new tcp_client_handler(io_srv_));
     return connections_.back();
}

