#include <iostream>
#include <map>
#include <chai_script_vm.h>

chai_script_vm::chai_script_vm(script_services *scriptContext) : _scriptServices{scriptContext} {

    state.add_global(chaiscript::var(_scriptServices), "GameBoy");
    state.add(chaiscript::fun(&script_services::print_string), "print");
    state.add(chaiscript::fun(&script_services::add_rect), "add_rect");
    state.add(chaiscript::fun(&script_services::add_text), "add_text");
    state.add(chaiscript::fun(&script_services::add_image), "add_image");
    state.add(chaiscript::fun(&script_services::read_8bit_value), "get_8bit_value");
    state.add(chaiscript::fun(&script_services::read_16bit_value), "get_16bit_value");
    state.add(chaiscript::fun(&script_services::queue_key), "queue_key");
    state.add(chaiscript::fun(&script_services::set_8bit_value), "set_8bit_value");
    state.add(chaiscript::fun(&script_services::set_16bit_value), "set_16bit_value");
    state.add(chaiscript::fun(&script_services::register_command), "register_console_command");
}

void chai_script_vm::registerConstants() const {

}


void chai_script_vm::activate() {
    state.eval("activate();");
}

void chai_script_vm::tick() {
    state.eval("tick();");
}

void chai_script_vm::loadScript(const std::string &script) {
    state.eval(script);
    if (script.find("onLoad") == std::string::npos) {
        state.eval("def onLoad(){}\n");
    }
    state.eval("onLoad();");
}

bool chai_script_vm::handleUnhandledCommand(const std::string &command, std::vector<std::string> args) {
    try {
        auto handleCommand = state.eval<std::function<bool(const std::string &, std::vector<std::string>)>>("handleCommand");
        return handleCommand(command, args);
    }
    catch (chaiscript::exception::eval_error &e) {
        std::cerr << "Failed to evaluate handleCommand" << std::endl;
    }

    return false;
}


