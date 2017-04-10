#pragma once

#include <string>
#include <memory>
#include <map>
#include "script_vm.h"

class script_manager {
public:
    void add_vm(const std::string &name, script_vm *vm);
    void remove_vm(const std::string &name);

    void tick();
    void activate();

    bool handleUnhandledCommand(const std::string &command, std::vector<std::string> args);

private:
    std::map<std::string, std::unique_ptr<script_vm>> _vms;
};
