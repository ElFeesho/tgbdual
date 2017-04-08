//
// Created by Christopher Sawczuk on 08/04/2017.
//

#include "script_manager.h"

void script_manager::add_vm(const std::string &name, script_vm *vm) {
    _vms.emplace(name, std::unique_ptr<script_vm>(vm));
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

void script_manager::remove_vm(const std::string &name) {
    if (_vms.find(name) != _vms.end())
    {
        _vms.erase(_vms.find(name));
    }
}
