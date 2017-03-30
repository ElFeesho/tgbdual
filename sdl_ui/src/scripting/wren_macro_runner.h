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
    wren_macro_runner(script_context &context);

    void tick() override;
    void activate() override;
    void loadScript(const std::string &scriptFile) override;

private:
    std::unique_ptr<WrenVM, void(*)(WrenVM*)> _wrenVm;
};

