#ifndef ASIO_HANDLER_BASE_HPP
#define ASIO_HANDLER_BASE_HPP

#include <string>
#include <iostream>

class handler_base
{
public:
    
    handler_base()
    {
        
    }
    
    virtual bool on_start(bool success = true) { return true; };
    
    virtual bool on_stop(bool success = true) { return true; };
    
    virtual bool handle_notify(int level, std::string message)
    {
        std::cout << "level " << level << " notification: " << message << std::endl;
        
    };
    

};

#endif
