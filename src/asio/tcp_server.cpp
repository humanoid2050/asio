#include "asio/tcp_server.hpp"

using boost::asio::ip::tcp;

tcp_server::tcp_server(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
    : asio_connection(io_service, std::move(description)), acceptor_(io_service)
{
    
}

bool tcp_server::start()
{
    if (running_.test_and_set(std::memory_order_acquire)) return false;
    tcp::endpoint ep(tcp::v4(), std::static_pointer_cast<socketDescription>(description_)->get_port());
    acceptor_.open(tcp::v4());
    acceptor_.bind(ep);
    acceptor_.listen();
    
    //server handler is responsible for providing a pointer to an object which will provide the connection API
    //this may be the server handler itself, or a new object specificially created for that new connection
    return start_accept();
    
}

bool tcp_server::start_accept()
{
    if (auto h = handler_.lock()) 
    {
        std::shared_ptr<tcp_client_handler> sh = h->get_connection_handler();
        acceptor_.async_accept(sh->get_socket(), 
                            boost::bind(&tcp_server::handle_accept,this,std::move(sh),boost::asio::placeholders::error));
        return true;
    }
    return false;
}

void tcp_server::handle_accept(std::shared_ptr<tcp_client_handler> sh, const boost::system::error_code& err)
{
    if (!err)
    {
        //if no errors, kickstart the connection
        sh->kickstart(err);
    }
    if (err != boost::asio::error::operation_aborted)
    {
        //if the operation was not aborted, get ready for the next connection
        start_accept();
    } 
}

bool tcp_server::stop()
{
    acceptor_.cancel();
    acceptor_.close();
    running_.clear(std::memory_order_release);
}

