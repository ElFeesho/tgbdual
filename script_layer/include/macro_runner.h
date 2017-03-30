#pragma once

#include <string>

class macro_runner {
public:
    virtual ~macro_runner() {}

    virtual void tick() = 0;
    virtual void activate() = 0;
    virtual void loadScript(const std::string &) = 0;
};