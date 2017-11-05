#include "asio.hpp"

//#include "asio/connection_handler.hpp"


int main()
{
    
    boost::asio::io_service io_s;
    
    std::unique_ptr<deviceDescription> desc(new serialDescription("/dev/ttyUSB0",serialDescription::baudRate::BAUD115200));
    
    deviceFactory::make_device(io_s,std::move(desc));

}
