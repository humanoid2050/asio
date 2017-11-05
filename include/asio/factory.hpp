#ifndef ASIO_FACTORY_HPP
#define ASIO_FACTORY_HPP


#include <memory>
#include "asio/serial_description.hpp"
#include "asio/socket_description.hpp"
#include "asio/timer_description.hpp"

#include "asio/serial.hpp"

#include "asio/tcp_client.hpp"
#include "asio/tcp_server.hpp"
#include "asio/udp_client.hpp"
#include "asio/udp_server.hpp"

#include "asio/local_stream_client.hpp"
#include "asio/local_dgram_client.hpp"
#include "asio/local_stream_server.hpp"
#include "asio/local_dgram_server.hpp"

#include "asio/timer.hpp"


class deviceFactory
{
public:
    static std::unique_ptr<asio_device> make_device(boost::asio::io_service & service, std::unique_ptr<deviceDescription> description)
    {
        
        switch (description->get_type())
        {
        case deviceDescription::deviceType::SERIAL:
            return std::unique_ptr<asio_device> (new asio_serial(service, std::move(description)));
        case deviceDescription::deviceType::SOCKET:
            //mask: 0 LOCAL, 1 IP; 0 STREAM, 2 DGRAM; 0 CLIENT, 4 SERVER
            switch (static_cast<uint8_t>(static_cast<socketDescription*>(description.get())->get_domain()) | 
                    static_cast<uint8_t>(static_cast<socketDescription*>(description.get())->get_protocol()) | 
                    static_cast<uint8_t>(static_cast<socketDescription*>(description.get())->get_roll()))
            {
            case 0:
                return std::unique_ptr<asio_device>(new local_stream_client(service, std::move(description)));
            case 1:
                return std::unique_ptr<asio_device>(new tcp_client(service, std::move(description)));
            case 2:
                return std::unique_ptr<asio_device>(new local_dgram_client(service, std::move(description)));
            case 3:
                return std::unique_ptr<asio_device>(new udp_client(service, std::move(description)));
            case 4:
                return std::unique_ptr<asio_device>(new local_stream_server(service, std::move(description)));
            case 5:
                return std::unique_ptr<asio_device>(new tcp_server(service, std::move(description)));
            case 6:
                return std::unique_ptr<asio_device>(new local_dgram_server(service, std::move(description)));
            case 7:
                return std::unique_ptr<asio_device>(new udp_server(service, std::move(description)));
            }
        case deviceDescription::deviceType::TIMER:
            return std::unique_ptr<asio_device>(new asio_timer(service, std::move(description)));
            break;
        case deviceDescription::deviceType::UNSET:
            throw std::runtime_error("cannot implement a device from a description of type UNSET");
        }
    }
};

#endif