#include "asio.hpp"
#include <memory>

class connection : public connection_handler, public std::enable_shared_from_this<connection_handler>
{
public:
    connection()
    {
        set_log_threshold(log_level::DEEP_DEBUG);
    }
    
    void connect(boost::asio::io_service& io_s, std::unique_ptr<deviceDescription> desc)
    {
        device_ = deviceFactory::make_connection(io_s,std::move(desc), shared_from_this());
    }
    
    bool on_receive(boost::asio::streambuf& msg_stream) 
    {
        
        std::istream is(&msg_stream);
        
        while (is.good())
        {
            std::string s;
            is >> s;
            notify(log_level::INFO,s);
        }
        device_->start_receive();
    }
    
    std::unique_ptr<asio_connection> device_;
};


int main()
{
    
    boost::asio::io_service io_s;
    
    std::unique_ptr<deviceDescription> desc(new socketDescription("localhost", 5000, socketDescription::protocol::STREAM, socketDescription::roll::CLIENT));
    
    auto s = std::make_shared<connection>();
    
    s->connect(io_s, std::move(desc));
    
    
    //auto h = std::make_shared<connection_handler>(io_s,std::move(desc));
    
    //deviceFactory::make_connection(io_s,std::move(desc),h);

    io_s.run();
}
