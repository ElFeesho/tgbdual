#include "gb_sys_command_source.h"

gb_sys_command_source::gb_sys_command_source(tgb::sys_command_source *cmdSource,
        gb_sys_command_source::sys_command saveState, gb_sys_command_source::sys_command loadState,
        gb_sys_command_source::sys_command toggleSpeed, gb_sys_command_source::sys_command quit,
        gb_sys_command_source::sys_command activateScript, gb_sys_command_source::sys_command openConsole)
    :
        _cmdSource{cmdSource},
        _saveState{saveState},
        _loadState{loadState},
        _toggleSpeed{toggleSpeed},
        _quit{quit},
        _activateScript{activateScript},
        _openConsole{openConsole}
{

}

void gb_sys_command_source::update() {
    _cmdSource->provideSysCommand(_saveState, _loadState, _toggleSpeed, _quit, _activateScript, _openConsole);
}
