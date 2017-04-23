#pragma once

#include <functional>

namespace tgb {
    class sys_command_source {

    public:
        virtual ~sys_command_source() = default;

        virtual void provideSysCommand(std::function<void()> saveState, std::function<void()> loadState,
                                       std::function<void()> toggleSpeed, std::function<void()> quit,
                                       std::function<void()> activateScript, std::function<void()> openConsole) = 0;
    };
}