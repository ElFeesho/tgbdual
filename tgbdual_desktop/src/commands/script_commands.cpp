#include "script_commands.h"

#include <wren_script_vm.h>
#include <lua_script_vm.h>
#include <io/file_buffer.h>

void registerScriptCommands(tgbdual &tgb) {
    tgb.addConsoleCommand("load_script", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            std::string &file = args[0];
            if (file.find(".wren") != std::string::npos) {

                wren_script_vm *wrenVm = new wren_script_vm(tgb.getScriptServices());
                try {
                    wrenVm->loadScript(file_buffer{file});
                    tgb.removeVm(file);
                    tgb.addVm(file, wrenVm);
                    tgb.addConsoleOutput("Loaded " + file + " wren script");
                }
                catch (std::domain_error &e) {
                    tgb.addConsoleErrorOutput(e.what());
                    delete wrenVm;
                }
            } else if (file.find(".lua") != std::string::npos) {
                lua_script_vm *luaVm = new lua_script_vm(tgb.getScriptServices());
                try {
                    luaVm->loadScript((file_buffer{file}));
                    tgb.removeVm(file);
                    tgb.addVm(file, luaVm);
                    tgb.addConsoleOutput("Loaded " + file + " lua script");
                }
                catch (std::domain_error &e) {
                    tgb.addConsoleErrorOutput(e.what());
                    delete luaVm;
                }
            } else {
                tgb.addConsoleErrorOutput("Cannot load " + file);
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
