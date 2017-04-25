#include "script_commands.h"

#include <wren_script_vm.h>
#include <lua_script_vm.h>

void registerScriptCommands(tgbdual &tgb) {
    tgb.getConsole().addCommand("load_script", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            std::string &file = args[0];
            if (file.find(".wren") != std::string::npos) {

                wren_script_vm *wrenVm = new wren_script_vm(tgb.getScriptServices());
                try {
                    wrenVm->loadScript(file_buffer{file});
                    tgb.getScriptManager().remove_vm(file);
                    tgb.getScriptManager().add_vm(file, wrenVm);
                    tgb.getConsole().addOutput("Loaded " + file + " wren script");
                }
                catch (std::domain_error &e) {
                    tgb.getConsole().addError(e.what());
                    delete wrenVm;
                }
            } else if (file.find(".lua") != std::string::npos) {
                lua_script_vm *luaVm = new lua_script_vm(tgb.getScriptServices());
                try {
                    luaVm->loadScript((file_buffer{file}));
                    tgb.getScriptManager().remove_vm(file);
                    tgb.getScriptManager().add_vm(file, luaVm);
                    tgb.getConsole().addOutput("Loaded " + file + " lua script");
                }
                catch (std::domain_error &e) {
                    tgb.getConsole().addError(e.what());
                    delete luaVm;
                }
            } else {
                tgb.getConsole().addError("Cannot load " + file);
            }
        } else {
            tgb.getConsole().addError("Usage: load_script [file]");
        }
    });

    tgb.getConsole().addCommand("unload_script", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            tgb.getScriptManager().remove_vm(args[0]);
        } else {
            tgb.getConsole().addError("Usage: unload_script [loaded script file]");
        }
    });
}
