#ifndef ASIO_DGRAM_HANDLER_HPP
#define ASIO_DGRAM_HANDLER_HPP


class dgram_handler : public connection_handler
{
    virtual bool handle_receive(std::vector<uint8_t> msg_stream);
}

#endif
