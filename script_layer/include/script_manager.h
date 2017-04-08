#pragma once

#include <string>
#include <map>
#include "macro_runner.h"

class script_manager {
public:
    void add_vm(const std::string &name, macro_runner *vm);

    void tick();
    void activate();

    bool handleUnhandledCommand(const std::string &command, std::vector<std::string> args);

private:
    std::map<std::string, std::unique_ptr<macro_runner>> _vms;
};
