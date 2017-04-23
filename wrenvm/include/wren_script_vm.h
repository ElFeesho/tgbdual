#pragma once

#include <script_vm.h>
#include <script_context.h>
#include <memory>

#include <wren.hpp>

class wren_script_vm : public script_vm {
public:
    using wrenvm_holder = std::unique_ptr<WrenVM, void(*)(WrenVM*)>;

    explicit wren_script_vm(script_services *context);

    void tick() override;
    void activate() override;
    void loadScript(const std::string &scriptFile) override;

    bool handleUnhandledCommand(const std::string &command, std::vector<std::string> args) override;

private:
    wrenvm_holder _wrenVm;

    void invokeWrenMethod(const std::string &methodName);
};

