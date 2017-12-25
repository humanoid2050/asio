#ifndef ASIO_UDP_CLIENT_HPP
#define ASIO_UDP_CLIENT_HPP


class udp_client : public asio_connection
{
public:
    udp_client(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description))
    {
        
    }
    
};

#endif
