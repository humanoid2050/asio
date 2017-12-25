#ifndef ASIO_SERVER_HANDLER_HPP
#define ASIO_SERVER_HANDLER_HPP

#include "asio/connection_handler.hpp"

class server_handler : public connection_handler, public std::enable_shared_from_this<server_handler>
{
public:
    std::shared_ptr<connection_handler> get_connection_handler()
    {
        return shared_from_this();
    }
    
};

#endif
