#pragma once

#include <memory>

#include "buffer.h"

class memory_buffer : buffer
{
public:
    void alloc(uint32_t length);

    uint32_t length() override;

    operator const char*() override {
        return (const char *)_buffer.get();
    }

    operator uint8_t*() override {
        return _buffer.get();
    }
private:
    uint32_t _length;
    std::unique_ptr<uint8_t[]> _buffer { nullptr };
};