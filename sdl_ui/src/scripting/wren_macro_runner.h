//
// Created by chris on 30/03/17.
//

#pragma once

#include <macro_runner.h>
#include <script_context.h>
#include <memory>

#include <wren.hpp>

class wren_macro_runner : public macro_runner {
public:
    using wrenvm_holder = std::unique_ptr<WrenVM, void(*)(WrenVM*)>;

    wren_macro_runner(script_context &context);

    void tick() override;
    void activate() override;
    void loadScript(const std::string &scriptFile) override;

    bool handleUnhandledCommand(const std::string &command, std::vector<std::string> args) override;

private:
    wrenvm_holder _wrenVm;
};

