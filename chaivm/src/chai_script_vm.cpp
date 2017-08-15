#include <iostream>
#include <map>
#include <chai_script_vm.h>

class ChaiScriptBridge {
public:
    ChaiScriptBridge(script_services *scriptServices) : _scriptServices{scriptServices} {

    }

    void print_string(const std::string &msg) {
        _scriptServices->print_string(msg);
    }

    void set_16bit_value(uint32_t address, uint16_t value) {
        _scriptServices->set_16bit_value(address, value);
    }

    void set_8bit_value(uint32_t address, uint8_t value) {
        _scriptServices->set_8bit_value(address, value);
    }

    uint8_t read_8bit_value(uint32_t address) {
        return _scriptServices->read_8bit_value(address);
    }

    uint16_t read_16bit_value(uint32_t address) {
        return _scriptServices->read_16bit_value(address);
    }

    void add_image(const std::string &name, int16_t x, int16_t y) {
        _scriptServices->add_image(name, x, y);
    }

    void add_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t stroke, uint32_t fill) {
        _scriptServices->add_rect(x, y, w, h, stroke, fill);
    }

    void add_text(const std::string &message, int16_t x, int16_t y) {
        _scriptServices->add_text(message, x, y);
    }

    void queue_key(uint8_t key, uint32_t when, uint32_t duration) {
        _scriptServices->queue_key(key, when, duration);
    }

    void register_command(const std::string &cmdName, std::function<void(std::vector<chaiscript::Boxed_Value>)> cmd) {
        _scriptServices->register_command(cmdName, [=](std::vector<std::string> args) {
            std::vector<chaiscript::Boxed_Value> boxedArgs;
            std::transform(args.begin(), args.end(), std::back_inserter(boxedArgs), [](const std::string &arg) { return chaiscript::Boxed_Value(arg); });
            cmd(boxedArgs);
        });
    }

private:
    script_services *_scriptServices;
};

chai_script_vm::chai_script_vm(script_services *scriptContext) {

    ChaiScriptBridge *bridge = new ChaiScriptBridge(scriptContext);
    state.add_global(chaiscript::var(bridge), "GameBoy");
    state.add(chaiscript::fun(&ChaiScriptBridge::print_string), "print");
    state.add(chaiscript::fun(&ChaiScriptBridge::add_rect), "add_rect");
    state.add(chaiscript::fun(&ChaiScriptBridge::add_text), "add_text");
    state.add(chaiscript::fun(&ChaiScriptBridge::add_image), "add_image");
    state.add(chaiscript::fun(&ChaiScriptBridge::read_8bit_value), "get_8bit_value");
    state.add(chaiscript::fun(&ChaiScriptBridge::read_16bit_value), "get_16bit_value");
    state.add(chaiscript::fun(&ChaiScriptBridge::queue_key), "queue_key");
    state.add(chaiscript::fun(&ChaiScriptBridge::set_8bit_value), "set_8bit_value");
    state.add(chaiscript::fun(&ChaiScriptBridge::set_16bit_value), "set_16bit_value");
    state.add(chaiscript::fun(&ChaiScriptBridge::register_command), "register_console_command");
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
        auto handleCommand = state.eval<std::function<bool(const std::string &, const std::vector<chaiscript::Boxed_Value> &)>>("handleCommand");

        std::vector<chaiscript::Boxed_Value> boxedArgs;
        std::transform(args.begin(), args.end(), std::back_inserter(boxedArgs), [](const std::string &arg) { return chaiscript::Boxed_Value(arg); });

        return handleCommand(command, boxedArgs);
    }
    catch (chaiscript::exception::eval_error &e) {
        std::cerr << "Failed to evaluate handleCommand " << e.what() << std::endl;
    }

    return false;
}


