#ifndef ASIO_DGRAM_HANDLER_HPP
#define ASIO_DGRAM_HANDLER_HPP

#include "asio/connection_handler.hpp"
#include "asio/dgram_connection.hpp"

template<class conn_T>
class dgram_handler : public connection_handler
{
public:
    dgram_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
    {
        device_ = std::unique_ptr<dgram_connection<conn_T>>(new dgram_connection<conn_T>(io_service, std::move(description)));
    }

    

    //virtual bool handle_receive(std::shared_ptr<std::vector<uint8_t>> msg) {}

    //std::shared_ptr<dgram_connection<conn_T>> device_;
};

typedef dgram_handler<boost::asio::ip::udp::socket> udp_client_handler;
typedef dgram_handler<boost::asio::local::datagram_protocol::socket> local_dgram_handler;

#endif
