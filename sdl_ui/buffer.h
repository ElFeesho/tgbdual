#pragma once

#include <cstdint>

class buffer {
public:
    virtual operator const char *() = 0;

    virtual operator uint8_t *() = 0;

    virtual uint32_t length() = 0;
};
