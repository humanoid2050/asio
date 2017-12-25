#ifndef ASIO_LOCAL_STREAM_SERVER_HPP
#define ASIO_LOCAL_STREAM_SERVER_HPP


class local_stream_server : public asio_connection
{
public:
    local_stream_server (boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description))
    {
        
    }
    
};

#endif
