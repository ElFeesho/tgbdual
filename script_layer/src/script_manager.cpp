//
// Created by Christopher Sawczuk on 08/04/2017.
//

#include "script_manager.h"

void script_manager::add_vm(const std::string &name, macro_runner *vm) {
    _vms.emplace(name, std::unique_ptr<macro_runner>(vm));
}

void script_manager::tick() {
    for(auto &vms : _vms) {
        vms.second->tick();
    }
}

void script_manager::activate() {
    for(auto &vms : _vms) {
        vms.second->activate();
    }
}

bool script_manager::handleUnhandledCommand(const std::string &command, std::vector<std::string> args) {
    bool handled = false;
    for(auto &vms : _vms) {
        if (vms.second->handleUnhandledCommand(command, args))
        {
            handled = true;
            break;
        }
    }
    return handled;
}