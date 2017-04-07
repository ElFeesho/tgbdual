#pragma once

#include <string>
#include <vector>

class macro_runner {
public:
    virtual ~macro_runner() {}

    virtual void tick() = 0;
    virtual void activate() = 0;
    virtual void loadScript(const std::string &) = 0;
    virtual bool handleUnhandledCommand(const std::string &command, std::vector<std::string> args) = 0;
};