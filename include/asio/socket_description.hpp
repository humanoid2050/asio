#ifndef ASIO_SOCKET_DESCRIPTION_HPP
#define ASIO_SOCKET_DESCRIPTION_HPP

#include "asio/device_description.hpp"


class socketDescription : public deviceDescription
{
public:
    enum class domain : uint8_t
    {
        LOCAL = 0,
        IP = 1
    };
    
    enum class protocol : uint8_t
    {
        STREAM = 0,
        DGRAM = 2
    };
    
    enum class roll : uint8_t
    {
        CLIENT = 0,
        SERVER = 4
    };
    
    socketDescription(std::string host, uint32_t port, protocol p, roll r) //IP
        : deviceDescription(deviceType::SOCKET,true), domain_(domain::IP), addr_(host), port_(port), protocol_(p), roll_(r)
    {
        
    }
    
    socketDescription(std::string address, protocol p, roll r) //LOCAL
        : deviceDescription(deviceType::SOCKET,true), domain_(domain::LOCAL), addr_(address), protocol_(p), roll_(r)
    {
        
    }
    
    void set_domain(domain dom)
    {
        domain_ = dom;
    }

    domain get_domain()
    {
        return domain_;
    }
    
    

    void set_host(std::string host)
    {
        addr_ = host;
    }

    std::string get_host()
    {
        return addr_;
    }

    void set_port(uint32_t port)
    {
        port_ = port;
    }

    uint32_t get_port()
    {
        return port_;
    }

    void set_local_addr(std::string addr)
    {
        addr_ = addr;
    }

    std::string get_local_addr()
    {
        return addr_;
    }

    void set_protocol(protocol proto)
    {
        protocol_ = proto;
    }

    protocol get_protocol()
    {
        return protocol_;
    }

    void set_roll(roll r)
    {
        roll_ = r;
    }

    roll get_roll()
    {
        return roll_;
    }

    void set_multicast(std::string group_addr)
    {
        multicast_addr_ = group_addr;
    }

    void clear_multicast()
    {
        multicast_addr_.clear();
    }

    bool is_multicast()
    {
        return !multicast_addr_.empty();
    }

    void set_broadcast(bool bcast)
    {
        broadcast_ = bcast;
    }

    void clear_broadcast()
    {
        broadcast_ = false;
    }

    bool is_broadcast()
    {
        return broadcast_;
    }



    
private:
    std::string addr_;
    uint32_t port_;
    domain domain_;
    protocol protocol_;
    roll roll_;
    
    bool broadcast_;
    std::string multicast_addr_;
    
};


#endif
