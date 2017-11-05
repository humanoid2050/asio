#ifndef ASIO_LOCAL_DGRAM_SERVER_HPP
#define ASIO_LOCAL_DGRAM_SERVER_HPP


class local_dgram_server : public asio_connection
{
public:
    local_dgram_server (boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description))
    {
        
    }
    
};

#endif
