#ifndef ASIO_TIMER_DESCRIPTION_HPP
#define ASIO_TIMER_DESCRIPTION_HPP

#include "asio/device_description.hpp"


class timerDescription : public deviceDescription
{
public:
    // interval sets rep rate, oneshot governs repetition
    timerDescription(std::chrono::nanoseconds interval, bool oneshot = false)
        : deviceDescription(deviceType::TIMER, !oneshot), interval_(interval), wall_time_(false)
    {
        
    }

    // interval sets a rep rate , and the ref time sets the anchor for the interval
    timerDescription(std::chrono::nanoseconds interval, std::chrono::time_point<std::chrono::steady_clock> ref_time)
        : deviceDescription(deviceType::TIMER, interval.count() != 0), interval_(interval), steady_ref_time_(ref_time), wall_time_(false)
    {
        
    }

    // interval sets a rep rate , and the ref time sets the anchor for the interval
    timerDescription(std::chrono::nanoseconds interval, std::chrono::time_point<std::chrono::system_clock> ref_time)
        : deviceDescription(deviceType::TIMER, interval.count() != 0), interval_(interval), sys_ref_time_(ref_time), wall_time_(true)
    {
        
    }
    
    
    bool is_wall_time()
    {
        return wall_time_;
    }

    void set_interval(std::chrono::nanoseconds interval)
    {
        interval_ = interval;
    }

    std::chrono::nanoseconds get_interval()
    {
        return interval_;
    }

    
    void set_ref_time(std::chrono::time_point<std::chrono::system_clock> ref_time);
    void set_ref_time(std::chrono::time_point<std::chrono::steady_clock> ref_time);
    
    
private:
    std::chrono::nanoseconds interval_;
    std::chrono::time_point<std::chrono::system_clock> sys_ref_time_;
    std::chrono::time_point<std::chrono::steady_clock> steady_ref_time_;
    bool wall_time_;
};


#endif
