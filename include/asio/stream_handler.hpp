#ifndef ASIO_STREAM_HANDLER_HPP
#define ASIO_STREAM_HANDLER_HPP

#include "asio/connection_handler.hpp"

class stream_handler : public connection_handler
{
public:
    //useufl if underlying connection uses the asio_read, asio_read_at, or
    //asio_read_until free functions returning stream_buf types
    virtual bool handle_receive(std::streambuf& msg_stream);
};

#endif
