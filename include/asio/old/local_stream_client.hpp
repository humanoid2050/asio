#ifndef ASIO_LOCAL_STREAM_CLIENT_HPP
#define ASIO_LOCAL_STREAM_CLIENT_HPP


class local_stream_client : public asio_connection
{
public:
    local_stream_client (boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description))
    {
        
    }
    
};

#endif
