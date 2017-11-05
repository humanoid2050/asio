#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP


//this class is specifically for binary data and raw memory moves
//do i need this? can i just operate on a std::vector directly?

class data_buffer
{
public:
    data_buffer(std::size_t size = 0)
    {
        data_.reserve(size);
    }
    
    bool add_data(void* data, std::size_t size)
    {
        data_.reserve(size);
        memcpy(&data_[0],data,size);
    }
    
private:
    std::vector<unsigned char> data_;
    
}


#endif
