#include "memory_buffer.h"

void memory_buffer::alloc(uint32_t length) {
    _length = length;
    _buffer = std::unique_ptr<uint8_t>(new uint8_t[length]);
}

uint32_t memory_buffer::length() {
    return _length;
}
