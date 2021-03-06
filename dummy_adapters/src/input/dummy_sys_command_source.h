#pragma once

#include <input/sys_command_source.h>

class dummy_sys_command_source : public tgb::sys_command_source {
public:
    void provideSysCommand(std::function<void()> saveState, std::function<void()> loadState, std::function<void()> toggleSpeed, std::function<void()> quit, std::function<void()> activateScript,
                           std::function<void()> openConsole) override;
};