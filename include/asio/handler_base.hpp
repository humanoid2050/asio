#ifndef ASIO_HANDLER_BASE_HPP
#define ASIO_HANDLER_BASE_HPP

#include <string>
#include <iostream>

enum class log_level : uint8_t {
    DEEP_DEBUG = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL   
}; 


class handler_base
{
public:
    handler_base() : log_threshold(log_level::DEBUG)
    {
        
    }
    
    void set_log_threshold(log_level lvl)
    {
        log_threshold = lvl;
    }
    
    virtual bool on_start(bool success = true) {}
    virtual bool on_stop(bool success = true) {}
        
    virtual void notify(log_level level, std::string message)
    {
        if (level < log_threshold) return;
        
        std::string lvl;
        switch(level)
        {
        case log_level::DEEP_DEBUG:
            lvl = "DEEP_DEBUG";
            break;
        case log_level::DEBUG:
            lvl = "DEBUG";
            break;
        case log_level::INFO:
            lvl = "INFO";
            break;
        case log_level::WARN:
            lvl = "WARN";
            break;
        case log_level::ERROR:
            lvl = "ERROR";
            break;
        case log_level::FATAL:
            lvl = "FATAL";
            break;
        }
        
        std::cout << lvl << ": " << message << std::endl;
    }

protected:
    log_level log_threshold;

};

#endif
