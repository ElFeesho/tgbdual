#pragma once

#include "buffer.h"

class memory_buffer : buffer
{
public:
    void alloc(uint32_t length);

    void dealloc();

    uint32_t length() override;

    operator const char*() override {
        return (const char *)_buffer;
    }

    operator uint8_t*() override {
        return _buffer;
    }
private:
    uint32_t _length;
    uint8_t *_buffer { nullptr };
};