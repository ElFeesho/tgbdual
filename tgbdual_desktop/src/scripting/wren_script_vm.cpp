//
// Created by chris on 30/03/17.
//

#include <wren.hpp>
#include <wren.h>

#include "wren_script_vm.h"

WrenForeignMethodFn boundFunction(std::string name, std::string signature);
void errorHandler(WrenVM *vm, WrenErrorType type, const char* module, int line, const char* message)
{
    std::cerr << "ERR: ";
    if (module != nullptr)
    {
        std::cerr << module << " ";
    }
    std::cerr << "Line " << line << " ";
    if (message != nullptr)
    {
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

    config.bindForeignClassFn = [](WrenVM* vm, const char* module, const char* className) -> WrenForeignClassMethods {
        WrenForeignClassMethods ctorDtor;
        ctorDtor.allocate = [](WrenVM *wrenVm) {
            std::cout << "BEEN CTORRED" << std::endl;
        };
        return ctorDtor;
    };

    config.bindForeignMethodFn = [](WrenVM* vm,
                                    const char* module,
                                    const char* className,
                                    bool isStatic,
                                    const char* signature) -> WrenForeignMethodFn {
        return boundFunction(className, signature);
    };

    config.errorFn = errorHandler;

    config.userData = &context;

    WrenVM *vm = wrenNewVM(&config);
    _wrenVm = wrenvm_holder(vm, wrenFreeVM);
}

void wren_script_vm::tick() {
    invokeWrenMethod("tick");
}

void wren_script_vm::invokeWrenMethod(const std::string &methodName) {
    wrenEnsureSlots(_wrenVm.get(), 1);
    wrenGetVariable(_wrenVm.get(), "main", methodName.c_str(), 0);
    if (wrenGetSlotType(_wrenVm.get(), 0) == WREN_TYPE_UNKNOWN) {
        wrenCall(_wrenVm.get(), wrenGetSlotHandle(_wrenVm.get(), 0));
    }
}

void wren_script_vm::activate() {
    invokeWrenMethod("activate");
}

void wren_script_vm::loadScript(const std::string &scriptFile) {
    auto result = wrenInterpret(_wrenVm.get(), scriptFile.c_str());
    if (result == WREN_RESULT_COMPILE_ERROR)
    {
        std::cerr << "Failed to compile wren script" << std::endl;
        script_context *context = (script_context *) wrenGetUserData(_wrenVm.get());
        context->print_string("Failed to compile wren script");
    }
    else if(result == WREN_RESULT_RUNTIME_ERROR) {
        std::cerr << "Failed to execute wren script" << std::endl;

        script_context *context = (script_context *) wrenGetUserData(_wrenVm.get());
        context->print_string("Failed to execute wren script");
    }

    invokeWrenMethod("onLoad");
}

bool wren_script_vm::handleUnhandledCommand(const std::string &command, std::vector<std::string> args) {
    wrenEnsureSlots(_wrenVm.get(), 1);
    wrenGetVariable(_wrenVm.get(), "main", "handleCommand", 0);
    WrenType type = wrenGetSlotType(_wrenVm.get(), 0);
    if (type == WREN_TYPE_NULL) {
        return false;
    }

    wrenEnsureSlots(_wrenVm.get(), 2);
    wrenSetSlotString(_wrenVm.get(), 1, command.c_str());
    wrenSetSlotNewList(_wrenVm.get(), 2);
    for(std::string &arg : args) {
        wrenEnsureSlots(_wrenVm.get(), 1);
        wrenSetSlotString(_wrenVm.get(), 3, arg.c_str());
        wrenInsertInList(_wrenVm.get(), 2, -1, 3);
        std::cout << "Inserted arg " << arg << std::endl;
    }

    WrenInterpretResult result = wrenCall(_wrenVm.get(), wrenGetSlotHandle(_wrenVm.get(), 0));
    if (result == WREN_RESULT_SUCCESS)
    {
        bool result = wrenGetSlotBool(_wrenVm.get(), 0);
        std::cout << "Result: " << result << std::endl;
        return result;
    }
    return false;
}

WrenForeignMethodFn boundFunction(std::string name, std::string signature) {
    if (name == "GameBoy")
    {
        if (signature == "print(_)")
        {
            return [](WrenVM *wrenVm) {
                script_context *context = (script_context *) wrenGetUserData(wrenVm);
                context->print_string(wrenGetSlotString(wrenVm, 1));
            };
        }
        else if(signature == "set8bit(_,_)")
        {
            return [](WrenVM *wrenVm) {
                script_context *context = (script_context *) wrenGetUserData(wrenVm);
                context->set_8bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1), (uint8_t) wrenGetSlotDouble(wrenVm, 2));
            };
        }
        else if(signature == "set16bit(_,_)")
        {
            return [](WrenVM *wrenVm) {
                script_context *context = (script_context *) wrenGetUserData(wrenVm);
                context->set_16bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1), (uint16_t) wrenGetSlotDouble(wrenVm, 2));
            };
        }
        else if(signature == "get8bit(_)")
        {
            return [](WrenVM *wrenVm) {
                script_context *context = (script_context *) wrenGetUserData(wrenVm);
                wrenSetSlotDouble(wrenVm, 0, context->read_8bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1)));
            };
        }
        else if(signature == "get16bit(_)")
        {
            return [](WrenVM *wrenVm) {
                script_context *context = (script_context *) wrenGetUserData(wrenVm);
                wrenSetSlotDouble(wrenVm, 0, context->read_16bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1)));
            };
        }
        else if(signature == "addImage(_,_,_)")
        {
            return [](WrenVM *wrenVm) {
                script_context *context = (script_context *) wrenGetUserData(wrenVm);
                context->add_image(wrenGetSlotString(wrenVm, 1), (int16_t) wrenGetSlotDouble(wrenVm, 2),
                                            (int16_t) wrenGetSlotDouble(wrenVm, 3));
            };
        }
        else if(signature == "addText(_,_,_)")
        {
            return [](WrenVM *wrenVm) {
                script_context *context = (script_context *) wrenGetUserData(wrenVm);
                context->add_text(wrenGetSlotString(wrenVm, 1), (int16_t) wrenGetSlotDouble(wrenVm, 2),
                                            (int16_t) wrenGetSlotDouble(wrenVm, 3));
            };
        }
        else if(signature == "addRect(_,_,_,_,_,_)")
        {
            return [](WrenVM *wrenVm) {
                script_context *context = (script_context *) wrenGetUserData(wrenVm);
                context->add_rect((int16_t) wrenGetSlotDouble(wrenVm, 1), (int16_t) wrenGetSlotDouble(wrenVm, 2),
                                           (int16_t) wrenGetSlotDouble(wrenVm, 3),
                                           (int16_t) wrenGetSlotDouble(wrenVm, 4),
                                           (uint32_t) wrenGetSlotDouble(wrenVm, 5),
                                           (uint32_t) wrenGetSlotDouble(wrenVm, 6));
            };
        }
        else if(signature == "queueKey(_,_,_)")
        {
            return [](WrenVM *wrenVm) {
                script_context *context = (script_context *) wrenGetUserData(wrenVm);
                context->queue_key((uint8_t) wrenGetSlotDouble(wrenVm, 1),
                                            (uint32_t) wrenGetSlotDouble(wrenVm, 2),
                                            (uint32_t) wrenGetSlotDouble(wrenVm, 3));
            };
        }
    }
    return [](WrenVM*){
        std::cerr << "UNIMPLEMENTED" << std::endl;
    };
}