#ifndef ASIO_DEVICE_DESCRIPTION_HPP
#define ASIO_DEVICE_DESCRIPTION_HPP

#include <chrono>
#include <utility>
#include <mutex>

class deviceDescription
{
public:
    enum class deviceType
    {
        SOCKET,
        SERIAL,
        TIMER,
        LOG_FILE,
        UNSET
    };
    
    deviceDescription(deviceType dev_type = deviceType::UNSET, bool persist = true)
        : type_(dev_type), persistent_(persist)
    {
        
    };
    
    
    void set_type(deviceType dev_type)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        type_ = dev_type;
    };
    
    deviceType get_type()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return type_;
    };
    
    void set_persistent(bool persist = true)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        persistent_ = persist;
    };
    
    bool get_persistent()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return persistent_;
    };
    
protected:
    deviceType type_;
    bool persistent_;
    std::mutex mtx_;
};

#endif
