#ifndef ASIO_CONNECTION_HANDLER_HPP
#define ASIO_CONNECTION_HANDLER_HPP

#include "asio/handler_base.hpp"
#include "asio/asio_connection.hpp"
#include "asio/factory.hpp"
#include "boost/bind.hpp"

class asio_connection;
//class deviceFactory;

class connection_handler : public handler_base
{
public:
    connection_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description) 
    {
        handle_notify(1,std::string("making connection handler"));
        device_ = deviceFactory::make_connection(io_service, std::move(description));
        handle_notify(1,std::string("making connection handler"));
        
        device_->bind_on_start(boost::bind(&connection_handler::on_start,this,_1));
        device_->bind_on_stop(boost::bind(&connection_handler::on_stop,this,_1));
        device_->bind_notify(boost::bind(&connection_handler::handle_notify,this,_1,_2));

        device_->bind_on_connect(boost::bind(&connection_handler::on_connect,this,_1));
        device_->bind_on_disconnect(boost::bind(&connection_handler::on_disconnect,this,_1));
        device_->bind_on_send(boost::bind(&connection_handler::on_send,this,_1,_2));
        device_->bind_on_receive(boost::bind(&connection_handler::on_receive,this,_1));
    }
    
    virtual bool on_connect(bool success = true) {};
    virtual bool on_disconnect(bool success = true) {};
    
    virtual bool on_send(uint32_t id = 0, bool success = true) {};
    
    //called directly, or chunked out 
    virtual bool on_receive(boost::asio::streambuf& msg_stream)
    {
        
    }
    
protected:
    std::shared_ptr<asio_connection> device_;
    
    
};

#endif
