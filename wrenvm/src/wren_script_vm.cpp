//
// Created by chris on 30/03/17.
//

#include <wren.hpp>
#include <iostream>

#include "wren_script_vm.h"

struct WrenVMMetadata {
    WrenVMMetadata(script_context *context) : scriptContext{context} {}

    script_context *scriptContext;

    static WrenVMMetadata *fromWrenVM(WrenVM *vm) {
        return (WrenVMMetadata *) wrenGetUserData(vm);
    }
};

WrenForeignMethodFn boundFunction(std::string name, std::string signature);

void populateWrenList(WrenVM *wrenVm, const std::vector<std::string> &args);

void errorHandler(WrenVM *vm, WrenErrorType type, const char *module, int line, const char *message) {
    std::cerr << "ERR: ";
    if (module != nullptr) {
        std::cerr << module << " ";
    }
    std::cerr << "Line " << line << " ";
    if (message != nullptr) {
        std::cerr << "Message: " << message;
    }

    std::cerr << std::endl;
}

wren_script_vm::wren_script_vm(script_context &context) : _wrenVm{nullptr, wrenFreeVM} {
    WrenConfiguration config;
    wrenInitConfiguration(&config);

    config.writeFn = [](WrenVM *vm, const char *text) {
        std::cout << "VM: " << text << std::endl;
    };

    config.bindForeignMethodFn = [](WrenVM *vm,
                                    const char *module,
                                    const char *className,
                                    bool isStatic,
                                    const char *signature) -> WrenForeignMethodFn {
        return boundFunction(className, signature);
    };

    config.errorFn = errorHandler;

    WrenVMMetadata *metadata = new WrenVMMetadata(&context);
    config.userData = metadata;

    WrenVM *vm = wrenNewVM(&config);
    _wrenVm = wrenvm_holder(vm, [](WrenVM *vm) {
        wrenGetUserData(vm);
        WrenVMMetadata *wrenVmMetadata = WrenVMMetadata::fromWrenVM(vm);
        wrenFreeVM(vm);
        delete wrenVmMetadata;
    });
}

void wren_script_vm::tick() {
    invokeWrenMethod("tick");
}

void wren_script_vm::invokeWrenMethod(const std::string &methodName) {
    wrenEnsureSlots(_wrenVm.get(), 1);
    wrenGetVariable(_wrenVm.get(), "main", methodName.c_str(), 0);
    if (wrenGetSlotType(_wrenVm.get(), 0) == WREN_TYPE_UNKNOWN) {
        WrenHandle *callHandle = wrenMakeCallHandle(_wrenVm.get(), "call()");
        wrenCall(_wrenVm.get(), callHandle);
        wrenReleaseHandle(_wrenVm.get(), callHandle);
    }
}

void wren_script_vm::activate() {
    invokeWrenMethod("activate");
}

void wren_script_vm::loadScript(const std::string &scriptFile) {
    std::string script = "foreign class GameBoy {\n"
            "    construct new() {}\n"
            "\n"
            "    foreign static print(text)\n"
            "    foreign static addRect(x, y, w, h, stroke, fill)\n"
            "    foreign static addImage(image, x, y)\n"
            "    foreign static addText(text, x, y)\n"
            "    foreign static get8bit(address)\n"
            "    foreign static get16bit(address)\n"
            "    foreign static set8bit(address, value)\n"
            "    foreign static set16bit(address, value)\n"
            "    foreign static registerConsoleCommand(name, func)\n"
            "\n"
            "    static get24bit(address) {\n"
            "        return get8bit(address) + (get8bit(address-1) << 8) + (get8bit(address-2) << 16)\n"
            "    }\n"
            "\n"
            "    static set24bit(address, value) {\n"
            "        set8bit(address, value & 0xff)\n"
            "        set8bit(address-1, (value >> 8) & 0xff)\n"
            "        set8bit(address-2, (value >> 16) & 0xff)\n"
            "    }\n"
            "\n"
            "    foreign static queueKey(key, delay, duration)\n"
            "\n"
            "    static KEY_A { 0x01 }\n"
            "    static KEY_B { 0x02 }\n"
            "    static KEY_SELECT { 0x04 }\n"
            "    static KEY_START { 0x08 }\n"
            "    static KEY_DOWN { 0x10 }\n"
            "    static KEY_UP { 0x20 }\n"
            "    static KEY_LEFT { 0x40 }\n"
            "    static KEY_RIGHT { 0x80 }\n"
            "}\n" + scriptFile;

    auto result = wrenInterpret(_wrenVm.get(), script.c_str());
    if (result == WREN_RESULT_COMPILE_ERROR) {
        std::cerr << "Failed to compile wren script" << std::endl;
        script_context *context = (script_context *) wrenGetUserData(_wrenVm.get());
        context->print_string("Failed to compile wren script");
    } else if (result == WREN_RESULT_RUNTIME_ERROR) {
        std::cerr << "Failed to execute wren script" << std::endl;

        script_context *context = (script_context *) wrenGetUserData(_wrenVm.get());
        context->print_string("Failed to execute wren script");
    }

    invokeWrenMethod("onLoad");
}

bool wren_script_vm::handleUnhandledCommand(const std::string &command, std::vector<std::string> args) {
    wrenEnsureSlots(_wrenVm.get(), 3);
    WrenHandle *handle = wrenMakeCallHandle(_wrenVm.get(), "call(_,_)");
    wrenGetVariable(_wrenVm.get(), "main", "handleCommand", 0);

    wrenSetSlotString(_wrenVm.get(), 1, command.c_str());
    wrenSetSlotNewList(_wrenVm.get(), 2);
    for (std::string &arg : args) {
        wrenEnsureSlots(_wrenVm.get(), 1);
        wrenSetSlotString(_wrenVm.get(), 3, arg.c_str());
        wrenInsertInList(_wrenVm.get(), 2, -1, 3);
    }

    WrenInterpretResult result = wrenCall(_wrenVm.get(), handle);
    wrenReleaseHandle(_wrenVm.get(), handle);

    return result == WREN_RESULT_SUCCESS && wrenGetSlotBool(_wrenVm.get(), 0);
}

WrenForeignMethodFn boundFunction(std::string name, std::string signature) {
    if (name == "GameBoy") {
        if (signature == "print(_)") {
            return [](WrenVM *wrenVm) {
                script_context *context = WrenVMMetadata::fromWrenVM(wrenVm)->scriptContext;
                context->print_string(wrenGetSlotString(wrenVm, 1));
            };
        } else if (signature == "set8bit(_,_)") {
            return [](WrenVM *wrenVm) {
                script_context *context = WrenVMMetadata::fromWrenVM(wrenVm)->scriptContext;
                context->set_8bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1), (uint8_t) wrenGetSlotDouble(wrenVm, 2));
            };
        } else if (signature == "set16bit(_,_)") {
            return [](WrenVM *wrenVm) {
                script_context *context = WrenVMMetadata::fromWrenVM(wrenVm)->scriptContext;
                context->set_16bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1), (uint16_t) wrenGetSlotDouble(wrenVm, 2));
            };
        } else if (signature == "get8bit(_)") {
            return [](WrenVM *wrenVm) {
                script_context *context = WrenVMMetadata::fromWrenVM(wrenVm)->scriptContext;
                wrenSetSlotDouble(wrenVm, 0, context->read_8bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1)));
            };
        } else if (signature == "get16bit(_)") {
            return [](WrenVM *wrenVm) {
                script_context *context = WrenVMMetadata::fromWrenVM(wrenVm)->scriptContext;
                wrenSetSlotDouble(wrenVm, 0, context->read_16bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1)));
            };
        } else if (signature == "addImage(_,_,_)") {
            return [](WrenVM *wrenVm) {
                script_context *context = WrenVMMetadata::fromWrenVM(wrenVm)->scriptContext;
                context->add_image(wrenGetSlotString(wrenVm, 1), (int16_t) wrenGetSlotDouble(wrenVm, 2),
                                   (int16_t) wrenGetSlotDouble(wrenVm, 3));
            };
        } else if (signature == "addText(_,_,_)") {
            return [](WrenVM *wrenVm) {
                script_context *context = WrenVMMetadata::fromWrenVM(wrenVm)->scriptContext;
                context->add_text(wrenGetSlotString(wrenVm, 1), (int16_t) wrenGetSlotDouble(wrenVm, 2),
                                  (int16_t) wrenGetSlotDouble(wrenVm, 3));
            };
        } else if (signature == "addRect(_,_,_,_,_,_)") {
            return [](WrenVM *wrenVm) {
                script_context *context = WrenVMMetadata::fromWrenVM(wrenVm)->scriptContext;
                context->add_rect((int16_t) wrenGetSlotDouble(wrenVm, 1), (int16_t) wrenGetSlotDouble(wrenVm, 2),
                                  (int16_t) wrenGetSlotDouble(wrenVm, 3),
                                  (int16_t) wrenGetSlotDouble(wrenVm, 4),
                                  (uint32_t) wrenGetSlotDouble(wrenVm, 5),
                                  (uint32_t) wrenGetSlotDouble(wrenVm, 6));
            };
        } else if (signature == "queueKey(_,_,_)") {
            return [](WrenVM *wrenVm) {
                script_context *context = WrenVMMetadata::fromWrenVM(wrenVm)->scriptContext;
                context->queue_key((uint8_t) wrenGetSlotDouble(wrenVm, 1),
                                   (uint32_t) wrenGetSlotDouble(wrenVm, 2),
                                   (uint32_t) wrenGetSlotDouble(wrenVm, 3));
            };
        } else if (signature == "registerConsoleCommand(_,_)") {
            return [](WrenVM *wrenVm) {
                script_context *context = WrenVMMetadata::fromWrenVM(wrenVm)->scriptContext;

                std::string command = wrenGetSlotString(wrenVm, 1);
                WrenHandle *consoleCommandHandle = wrenGetSlotHandle(wrenVm, 2);
                
                context->register_command(command, [=](std::vector<std::string> args) {
                    populateWrenList(wrenVm, args);
                    WrenHandle *callHandle = wrenMakeCallHandle(wrenVm, "call(_)");
                    wrenSetSlotHandle(wrenVm, 0, consoleCommandHandle);
                    wrenCall(wrenVm, callHandle);
                    wrenReleaseHandle(wrenVm, callHandle);
                });
            };
        }
    }
    return [](WrenVM *) {
        std::cerr << "UNIMPLEMENTED" << std::endl;
    };
}

void populateWrenList(WrenVM *wrenVm, const std::vector<std::string> &args) {
    wrenEnsureSlots(wrenVm, 1);
    wrenSetSlotNewList(wrenVm, 1);
    for (const std::string &arg : args) {
        wrenEnsureSlots(wrenVm, 1);
        wrenSetSlotString(wrenVm, 2, arg.c_str());
        wrenInsertInList(wrenVm, 1, -1, 2);
    }
}