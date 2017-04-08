#pragma once

#include <string>
#include <vector>

class script_vm {
public:
    virtual ~script_vm() {}

    virtual void tick() = 0;
    virtual void activate() = 0;
    virtual void loadScript(const std::string &) = 0;
    virtual bool handleUnhandledCommand(const std::string &command, std::vector<std::string> args) = 0;
};