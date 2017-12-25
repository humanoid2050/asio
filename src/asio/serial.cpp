#include "asio/serial.hpp"
#include "asio/serial_description.hpp"

asio_serial::asio_serial(boost::asio::io_service & io_service, std::unique_ptr<deviceDescription> description)
    : asio_connection(io_service, std::move(description)), socket_(io_service)
{
    
}

bool asio_serial::start()
{
    auto desc = std::static_pointer_cast<serialDescription>(description_);
    socket_.open(desc->get_device());
    socket_.set_option(boost::asio::serial_port::baud_rate(static_cast<uint32_t>(desc->get_baud())));
    //socket_.set_option();
    
    start_send();
    start_receive();
}

bool asio_serial::send(std::shared_ptr<std::vector<uint8_t>> msg)
{
    {
        std::lock_guard<std::mutex> lock(outbound_mutex_);
        outbound_msg_.push_back(msg);
    }
    start_send();
    return true;
}


bool asio_serial::start_send()
{
    if (sending_.test_and_set(std::memory_order_acquire)) return false;
    do_send();
    return true;
}

bool asio_serial::do_send()
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
    boost::asio::async_write(socket_, boost::asio::buffer(*msg), boost::bind(&asio_serial::handle_send, this, msg,
                             boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    return true;
}

void asio_serial::handle_send(std::shared_ptr<std::vector<uint8_t>> msg, const boost::system::error_code& err, size_t count)
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

void asio_serial::start_receive()
{
    boost::asio::async_read(socket_, buff_, boost::asio::transfer_at_least(1), boost::bind(&asio_serial::handle_receive, this, 
        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void asio_serial::handle_receive(const boost::system::error_code& err, std::size_t count)
{
    if (!err)
    {
        if (auto h = handler_.lock()) std::static_pointer_cast<serial_handler>(h)->handle_receive(buff_);
        start_receive();
    }
    else
    {
        restart();
    }
}

bool asio_serial::stop()
{
    socket_.cancel();
    socket_.close();
    connected_.clear(std::memory_order_release);
    if (auto h = handler_.lock()) std::static_pointer_cast<connection_handler>(h)->on_disconnect();
    running_.clear(std::memory_order_release);
}

bool asio_serial::restart()
{
    stop();
    start();
}
