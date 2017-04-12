//
// Created by Christopher Sawczuk on 11/04/2017.
//

#include "script_commands.h"

#include <scripting/wren_script_vm.h>
#include <io/file_buffer.h>
#include <scripting/lua_script_vm.h>

void registerScriptCommands(script_manager &scriptManager, console &console, script_context &context) {
    console.addCommand("load_script", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            std::string &file = args[0];
            if (file.find(".wren") != std::string::npos) {

                wren_script_vm *wrenVm = new wren_script_vm(context);
                try {
                    wrenVm->loadScript(file_buffer{file});
                    scriptManager.remove_vm(file);
                    scriptManager.add_vm(file, wrenVm);
                    console.addOutput("Loaded " + file + " wren script");
                }
                catch (std::domain_error &e) {
                    console.addError(e.what());
                    delete wrenVm;
                }
            } else if (file.find(".lua") != std::string::npos) {
                lua_script_vm *luaVm = new lua_script_vm(context);
                try {
                    luaVm->loadScript((file_buffer{file}));
                    scriptManager.remove_vm(file);
                    scriptManager.add_vm(file, luaVm);
                    console.addOutput("Loaded " + file + " lua script");
                }
                catch (std::domain_error &e) {
                    console.addError(e.what());
                    delete luaVm;
                }
            } else {
                console.addError("Cannot load " + file);
            }
        } else {
            console.addError("Usage: load_script [file]");
        }
    });

    console.addCommand("unload_script", [&](std::vector<std::string> args) {
        if (args.size() == 1) {
            scriptManager.remove_vm(args[0]);
        } else {
            console.addError("Usage: unload_script [loaded script file]");
        }
    });
}
