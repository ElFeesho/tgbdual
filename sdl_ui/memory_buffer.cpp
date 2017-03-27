#include "memory_buffer.h"

void memory_buffer::alloc(uint32_t length) {
    _length = length;
    _buffer = new uint8_t[length];
}

void memory_buffer::dealloc() {
    delete[] _buffer;
}

uint32_t memory_buffer::length() {
    return _length;
}
