#ifndef ASIO_SERIAL_HPP
#define ASIO_SERIAL_HPP

#include <asio/connection.hpp>


class asio_serial : public asio_connection
{
public:
    asio_serial(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : asio_connection(io_service, std::move(description)), serial_(io_service)
    {
        
    }
    
    virtual bool start()
    {
        auto desc = std::static_pointer_cast<serialDescription>(description_);
        serial_.open(desc->get_device());
        serial_.set_option(boost::asio::serial_port::baud_rate(static_cast<uint32_t>(desc->get_baud())));
        //serial_.set_option();
    }
    
    
    
    
private:
    
    
    boost::asio::serial_port serial_;
    
    using asio_connection::description_;
};

#endif
