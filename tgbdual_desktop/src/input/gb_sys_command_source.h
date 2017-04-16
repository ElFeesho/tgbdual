#pragma once

#include <functional>
#include "sys_command_source.h"

class gb_sys_command_source {
public:
    using sys_command = std::function<void()>;

    gb_sys_command_source(tgb::sys_command_source *cmdSource, sys_command saveState, sys_command loadState, sys_command toggleSpeed, sys_command quit, sys_command activateScript, sys_command openConsole);
    void update();
private:
    tgb::sys_command_source *_cmdSource;
    sys_command _saveState;
    sys_command _loadState;
    sys_command _toggleSpeed;
    sys_command _quit;
    sys_command _activateScript;
    sys_command _openConsole;
};