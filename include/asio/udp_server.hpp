#ifndef ASIO_UDP_SERVER_HPP
#define ASIO_UDP_SERVER_HPP



class udp_connection : public asio_connection
{
public:
    udp_connection(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description))
    {
        
    }
    
};



class udp_server : public asio_connection
{
public:
    udp_server(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description))
    {
        
    }
    
};

#endif
