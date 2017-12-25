#ifndef ASIO_SERIAL_HANDLER_HPP
#define ASIO_SERIAL_HANDLER_HPP

#include "asio/connection_handler.hpp"
#include "asio/stream_connection.hpp"

template<class conn_T>
class stream_handler : public connection_handler
{
public:
    stream_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
    {
        //device_ = std::unique_ptr<stream_connection<conn_T>>(new stream_connection<conn_T>(io_service, std::move(description)));
    }


    //useufl if underlying connection uses the asio_read, asio_read_at, or
    //asio_read_until free functions returning stream_buf types
    virtual bool handle_receive(std::streambuf& msg_stream) {}

    //std::shared_ptr<stream_connection<conn_T>> device_;
};

typedef stream_handler<boost::asio::ip::tcp::socket> tcp_client_handler;
typedef stream_handler<boost::asio::serial_port> serial_handler;
typedef stream_handler<boost::asio::local::stream_protocol::socket> local_stream_handler;

#endif
