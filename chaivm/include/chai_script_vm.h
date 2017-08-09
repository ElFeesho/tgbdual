#pragma once

#include <script_vm.h>
#include <osd_renderer.h>
#include <script_context.h>
#include <input_queue.h>

#include <memory>
#include <chaiscript/chaiscript.hpp>

class chai_script_vm : public script_vm {
public:
    explicit chai_script_vm(script_services *scriptServices);

    void loadScript(const std::string &script) override;

    void activate() override;

    void tick() override;

    bool handleUnhandledCommand(const std::string &command, std::vector<std::string> args) override;

private:
    script_services *_scriptServices;
    chaiscript::ChaiScript state;
};

