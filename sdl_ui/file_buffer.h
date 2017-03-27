#pragma once

#include <string>
#include <memory>
#include "buffer.h"

class file_buffer : buffer 
{
public:
    file_buffer(const std::string &name);
    uint32_t length();

    operator const char*() {
        return (const char *)_buffer.get();
    }

    operator uint8_t*() {
        return _buffer.get();
    }

    operator std::string() {
        return std::string((const char*)_buffer.get(), _length);
    }
    
private:
    uint32_t _length;
    std::unique_ptr<uint8_t> _buffer;
};

