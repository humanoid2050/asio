#ifndef ASIO_SERIAL_DESCRIPTION_HPP
#define ASIO_SERIAL_DESCRIPTION_HPP

#include "asio/device_description.hpp"


class serialDescription : public deviceDescription
{
public:
    enum class flowControl
    {
        NONE,
        SOFTWARE,
        HARDWARE
    };
    
    enum class parity
    {
        NONE,
        ODD,
        EVEN
    };
    
    enum class stopBits
    {
        ONE,
        ONEPTFIVE,
        TWO
    };
    
    enum class baudRate : uint32_t
    {
        BAUD0 = 0,
        BAUD50 = 50,
        BAUD75 = 75,
        BAUD110 = 110,
        BAUD134 = 134,
        BAUD150 = 150,
        BAUD200 = 200,
        BAUD300 = 300,
        BAUD600 = 600,
        BAUD1200 = 1200,
        BAUD1800 = 1800,
        BAUD2400 = 2400,
        BAUD4800 = 4800,
        BAUD9600 = 9600,
        BAUD19200 = 19200,
        BAUD38400 = 38400,
        BAUD57600 = 57600,
        BAUD115200 = 115200,
        BAUD230400 = 230400
        
    };
    
    serialDescription(std::string device, baudRate baud, uint8_t char_size = 8, parity p = parity::NONE, 
            stopBits sb = stopBits::ONE, flowControl fc = flowControl::NONE)
        : deviceDescription(deviceDescription::deviceType::SERIAL,true), device_(device), baud_(baud), char_size_(char_size), 
            parity_(p), stop_bits_(sb), flow_control_(fc)
    {
        
    }
    
    void set_device(std::string device)
    {
        
        std::lock_guard<std::mutex> lock(mtx_);
        device_ = device;
    }

    std::string get_device()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return device_;
    }

    void set_baud(baudRate baud)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        baud_ = baud;
    }

    baudRate get_baud()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return baud_;
    }

    void set_flow_control(flowControl flow_control)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        flow_control_ = flow_control;
    }

    flowControl get_flow_control()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return flow_control_;
    }
    
    
    void set_parity(parity bit_parity)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        parity_ = bit_parity;
    }

    parity get_parity()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return parity_;
    }

    void set_stop_bits(stopBits bits)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        stop_bits_ = bits;
    }

    stopBits get_stop_bits()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return stop_bits_;
    }

    void set_char_size(uint8_t char_size)
    {
        std::lock_guard<std::mutex> lock(mtx_);
        char_size_ = char_size;
    }

    uint8_t get_char_size()
    {
        std::lock_guard<std::mutex> lock(mtx_);
        return char_size_;
    }
    
private:
    
    std::string device_;
    baudRate baud_;
    uint8_t char_size_;
    flowControl flow_control_;
    parity parity_;
    stopBits stop_bits_;
    
    
};


#endif
