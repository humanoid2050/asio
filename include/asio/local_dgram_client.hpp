#ifndef ASIO_LOCAL_DGRAM_CLIENT_HPP
#define ASIO_LOCAL_DGRAM_CLIENT_HPP

class local_dgram_client : public asio_connection
{
public:
    local_dgram_client (boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description))
    {
        
    }
    
};

#endif
