#include "asio.hpp"

int main()
{
    
    boost::asio::io_service io_s;
    
    std::unique_ptr<deviceDescription> desc(new serialDescription("/dev/ttyUSB0",serialDescription::baudRate::BAUD115200));
    
    auto h = std::make_shared<connection_handler>(io_s,std::move(desc));
    
    //deviceFactory::make_connection(io_s,std::move(desc),h);

    io_s.run();
}
