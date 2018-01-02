#ifndef ASIO_FACTORY_HPP
#define ASIO_FACTORY_HPP


#include <memory>

#include "asio/asio_connection.hpp"
#include "asio/stream_connection.hpp"
#include "asio/stream_server.hpp"
#include "asio/dgram_connection.hpp"
#include "asio/asio_timer.hpp"


class deviceFactory
{
public:
    static std::unique_ptr<asio_connection> make_connection(boost::asio::io_service & service, 
                                                            std::unique_ptr<deviceDescription> description, 
                                                            std::shared_ptr<connection_handler>handler)
    {
        switch (description->get_type())
        {
        case deviceDescription::deviceType::SERIAL:
            handler->notify(log_level::DEEP_DEBUG,"deviceFactory making serial_device");
            return std::unique_ptr<asio_connection>(new serial_device(service, std::move(description), std::move(handler)));
        case deviceDescription::deviceType::SOCKET:
            //mask: 0 LOCAL, 1 IP; 0 STREAM, 2 DGRAM; 0 CLIENT, 4 SERVER
            switch (static_cast<uint8_t>(static_cast<socketDescription*>(description.get())->get_domain()) | 
                    static_cast<uint8_t>(static_cast<socketDescription*>(description.get())->get_protocol()) | 
                    static_cast<uint8_t>(static_cast<socketDescription*>(description.get())->get_roll()))
            {
            case 0:
                return std::unique_ptr<asio_connection>(new local_stream_client(service, std::move(description), std::move(handler)));
            case 1:
                return std::unique_ptr<asio_connection>(new tcp_client(service, std::move(description), std::move(handler)));
            case 2:
                return std::unique_ptr<asio_connection>(new local_dgram_client(service, std::move(description), std::move(handler)));
            case 3:
                return std::unique_ptr<asio_connection>(new udp_client(service, std::move(description), std::move(handler)));
            case 4:
                return std::unique_ptr<asio_connection>(new local_stream_server(service, std::move(description), std::move(handler)));
            case 5:
                return std::unique_ptr<asio_connection>(new tcp_server(service, std::move(description), std::move(handler)));
            case 6:
                return std::unique_ptr<asio_connection>(new local_dgram_server(service, std::move(description), std::move(handler)));
            case 7:
                return std::unique_ptr<asio_connection>(new udp_server(service, std::move(description), std::move(handler)));
            }
        
        case deviceDescription::deviceType::UNSET:
            throw std::runtime_error("cannot implement a connection from a description of type UNSET");
        case deviceDescription::deviceType::TIMER:
            throw std::runtime_error("cannot implement a connection from a TIMER description");
        }
    }
    
    
    static std::unique_ptr<asio_timer> make_timer(boost::asio::io_service & service,
                                                  std::unique_ptr<deviceDescription> description,
                                                  std::shared_ptr<connection_handler>handler = std::shared_ptr<connection_handler>())
    {
        switch (description->get_type())
        {
        case deviceDescription::deviceType::TIMER:
            return std::unique_ptr<asio_timer>(new asio_timer(service, std::move(description)));
            break;
        case deviceDescription::deviceType::UNSET:
            throw std::runtime_error("cannot implement a timer from a description of type UNSET");
        default:
            throw std::runtime_error("cannot implement a timer from the description object provided");
        }
    }
};

#endif
