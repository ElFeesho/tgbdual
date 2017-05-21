#include "script_commands.h"

#include <wren_script_vm.h>
#include <lua_script_vm.h>
#include <io/file_buffer.h>

enum class ScriptEngineType {
    Lua,
    Wren,
    Unknown
};

ScriptEngineType scriptEngineTypeForFile(const std::string &file) {
    if (file.find(".wren") != std::string::npos) {
        return ScriptEngineType::Wren;
    }

    if (file.find(".lua") != std::string::npos) {
        return ScriptEngineType::Lua;
    }

    return ScriptEngineType::Unknown;
}

script_vm *createVmForType(tgbdual &tgb, ScriptEngineType type) {
    if (type == ScriptEngineType::Wren) {
        return new wren_script_vm{tgb.getScriptServices()};
    }

    return new lua_script_vm{tgb.getScriptServices()};
}

void registerScriptCommands(tgbdual &tgb) {
    tgb.addConsoleCommand("load_script", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            std::string &file = args[0];

            ScriptEngineType type = scriptEngineTypeForFile(file);
            if (type == ScriptEngineType::Unknown) {
                tgb.addConsoleErrorOutput("Cannot load " + file);
            } else {
                script_vm *vm = createVmForType(tgb, type);
                try {
                    vm->loadScript(file_buffer{file});
                    tgb.removeVm(file);
                    tgb.addVm(file, vm);
                    tgb.addConsoleOutput("Loaded script: " + file);
                }
                catch (std::domain_error &e) {
                    tgb.addConsoleErrorOutput(e.what());
                    delete vm;
                }
            }
        } else {
            tgb.addConsoleErrorOutput("Usage: load_script [file]");
        }
    });

    tgb.addConsoleCommand("unload_script", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            tgb.removeVm(args[0]);
        } else {
            tgb.addConsoleErrorOutput("Usage: unload_script [loaded script file]");
        }
    });
}
