#include "asio/tcp_client.hpp"
#include "asio/socket_description.hpp"

using boost::asio::ip::tcp;

tcp_client::tcp_client(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
    : asio_connection(io_service, std::move(description)), sending_(ATOMIC_FLAG_INIT), resolver_(io_service), socket_(io_service)
{
    
}

tcp::socket& tcp_client::socket()
{
    return socket_;
}

bool tcp_client::resolve()
{
    if (running_.test_and_set(std::memory_order_acquire)) return false;
    auto desc = std::static_pointer_cast<socketDescription>(description_);
    tcp::resolver::query query(desc->get_host(), std::to_string(desc->get_port()));
    resolver_.async_resolve(query, boost::bind(&tcp_client::handle_resolve, this, 
                            boost::asio::placeholders::error, boost::asio::placeholders::iterator));
    return true;
}

void tcp_client::handle_resolve(const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator)
{
    if (!err)
    {
        // Attempt a connection to each endpoint in the list until we
        // successfully establish a connection.
        boost::asio::async_connect(socket_, endpoint_iterator, boost::bind(&tcp_client::handle_connect, this, boost::asio::placeholders::error));
    }
    else if (err != boost::asio::error::operation_aborted)
    {
        restart();
    }
}

void tcp_client::handle_connect(const boost::system::error_code& err)
{
    if (!err)
    {
        connected_.test_and_set(std::memory_order_acquire);
        if (auto h = handler_.lock()) std::static_pointer_cast<connection_handler>(h)->on_connect();
        start();
    }
    else if (err != boost::asio::error::operation_aborted)
    {
        restart();
    }
}

bool tcp_client::start()
{
    
    start_send();
    start_receive();
}

bool tcp_client::send(std::shared_ptr<std::vector<uint8_t>> msg)
{
    //critical operation is just the push
    {
        std::lock_guard<std::mutex> lock(outbound_mutex_);
        outbound_msg_.push_back(msg);
    }
    start_send();
    return true;
}

bool tcp_client::start_send()
{
    if (sending_.test_and_set(std::memory_order_acquire)) return false;
    do_send();
    return true;
}

bool tcp_client::do_send()
{
    std::shared_ptr<std::vector<uint8_t>> msg;
    //critial operations are the check, copy, and pop
    {
        std::lock_guard<std::mutex> lock(outbound_mutex_);
        if (outbound_msg_.empty())
        {
            sending_.clear(std::memory_order_release);
            return false;
        }
        msg = outbound_msg_.front();
        outbound_msg_.pop_front();
    }
    boost::asio::async_write(socket_, boost::asio::buffer(*msg), boost::bind(&tcp_client::handle_send, this, msg,
                             boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    return true;
}

void tcp_client::handle_send(std::shared_ptr<std::vector<uint8_t>> msg, const boost::system::error_code& err, size_t count)
{
    if (!err)
    {
        do_send();
    }
    else
    {
        restart();
    }
    
}

void tcp_client::start_receive()
{
    boost::asio::async_read(socket_, buff_, boost::asio::transfer_at_least(1), boost::bind(&tcp_client::handle_receive, this, 
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void tcp_client::handle_receive(const boost::system::error_code& err, std::size_t count)
{
    if (!err)
    {
        if (auto h = handler_.lock()) std::static_pointer_cast<tcp_client_handler>(h)->handle_receive(buff_);
        start_receive();
    }
    else
    {
        restart();
    }
}

bool tcp_client::stop()
{
    
    resolver_.cancel();
    socket_.shutdown(tcp::socket::shutdown_both);
    socket_.cancel();
    socket_.close();
    connected_.clear(std::memory_order_release);
    if (auto h = handler_.lock()) std::static_pointer_cast<connection_handler>(h)->on_disconnect();
    running_.clear(std::memory_order_release);
}

bool tcp_client::restart()
{
    stop();
    start();
}

tcp::socket& tcp_client::get_socket()
{
    return socket_;
}


