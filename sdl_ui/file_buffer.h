#pragma once

#include <string>

#include "buffer.h"

class file_buffer : buffer 
{
public:
    file_buffer(const std::string &name);
    ~file_buffer();
    uint32_t length();

    operator const char*() {
        return (const char *)_buffer;
    }

    operator uint8_t*() {
        return _buffer;
    }
    
private:
    uint32_t _length;
    uint8_t *_buffer;
};

