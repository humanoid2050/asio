#ifndef ASIO_STREAM_SERVER_HANDLER_HPP
#define ASIO_STREAM_SERVER_HANDLER_HPP

#include "asio/connection_handler.hpp"
#include "asio/stream_server.hpp"


//revist this class when designing unique client pattern
template<class conn_T>
class stream_server_handler : public connection_handler
{
    stream_server_handler(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
        : connection_handler(io_service,std::move(description)), io_svc_(io_service)
    {
        //device_ = deviceFactory::make_device(io_service, std::move(description)));
    }

    virtual std::shared_ptr<connection_handler> get_connection_handler() = 0;
    //{
         //connections_.emplace_back(new stream_connection<conn_T>(io_svc_));
         //return connections_.back();
    //}

    std::vector<connection_handler> connections_;
    boost::asio::io_service& io_svc_;
    
    //std::shared_ptr<asio_device> device_;
};





#endif
