//
// Created by chris on 30/03/17.
//

#include <wren.hpp>

#include "wren_macro_runner.h"

static std::map<WrenVM*, script_context*> contexts;
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

wren_macro_runner::wren_macro_runner(script_context &context) : _wrenVm{nullptr, wrenFreeVM} {
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

    WrenVM *vm = wrenNewVM(&config);
    _wrenVm = std::unique_ptr<WrenVM, void (*)(WrenVM*)>(vm, wrenFreeVM);

    contexts[_wrenVm.get()] = &context;
}

void wren_macro_runner::tick() {
    wrenEnsureSlots(_wrenVm.get(), 1);
    wrenGetVariable(_wrenVm.get(), "main", "tick", 0);
    wrenCall(_wrenVm.get(), wrenGetSlotHandle(_wrenVm.get(), 0));
}

void wren_macro_runner::activate() {
    wrenEnsureSlots(_wrenVm.get(), 1);
    wrenGetVariable(_wrenVm.get(), "main", "activate", 0);
    wrenCall(_wrenVm.get(), wrenGetSlotHandle(_wrenVm.get(), 0));
}

void wren_macro_runner::loadScript(const std::string &scriptFile) {
    auto result = wrenInterpret(_wrenVm.get(), scriptFile.c_str());
    if (result == WREN_RESULT_COMPILE_ERROR)
    {
        std::cerr << "Failed to compile wren script" << std::endl;
        contexts[_wrenVm.get()]->print_string("Failed to compile wren script");

    }
    else if(result == WREN_RESULT_RUNTIME_ERROR) {
        std::cerr << "Failed to execute wren script" << std::endl;
        contexts[_wrenVm.get()]->print_string("Failed to execute wren script");
    }

}

WrenForeignMethodFn boundFunction(std::string name, std::string signature) {
    if (name == "GameBoy")
    {
        if (signature == "print(_)")
        {
            return [](WrenVM *wrenVm) {
                contexts[wrenVm]->print_string(wrenGetSlotString(wrenVm, 1));
            };
        }
        else if(signature == "set8bit(_,_)")
        {
            return [](WrenVM *wrenVm) {
                contexts[wrenVm]->set_8bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1), (uint8_t) wrenGetSlotDouble(wrenVm, 2));
            };
        }
        else if(signature == "set16bit(_,_)")
        {
            return [](WrenVM *wrenVm) {
                contexts[wrenVm]->set_16bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1), (uint16_t) wrenGetSlotDouble(wrenVm, 2));
            };
        }
        else if(signature == "get8bit(_)")
        {
            return [](WrenVM *wrenVm) {
                wrenSetSlotDouble(wrenVm, 0, contexts[wrenVm]->read_8bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1)));
            };
        }
        else if(signature == "get16bit(_)")
        {
            return [](WrenVM *wrenVm) {
                wrenSetSlotDouble(wrenVm, 0, contexts[wrenVm]->read_16bit_value((uint32_t) wrenGetSlotDouble(wrenVm, 1)));
            };
        }
        else if(signature == "addImage(_,_,_)")
        {
            return [](WrenVM *wrenVm) {
                contexts[wrenVm]->add_image(wrenGetSlotString(wrenVm, 1), (int16_t) wrenGetSlotDouble(wrenVm, 2),
                                            (int16_t) wrenGetSlotDouble(wrenVm, 3));
            };
        }
        else if(signature == "addRect(_,_,_,_,_,_)")
        {
            return [](WrenVM *wrenVm) {
                contexts[wrenVm]->add_rect((int16_t) wrenGetSlotDouble(wrenVm, 1), (int16_t) wrenGetSlotDouble(wrenVm, 2),
                                           (int16_t) wrenGetSlotDouble(wrenVm, 3),
                                           (int16_t) wrenGetSlotDouble(wrenVm, 4),
                                           (uint32_t) wrenGetSlotDouble(wrenVm, 5),
                                           (uint32_t) wrenGetSlotDouble(wrenVm, 6));
            };
        }
        else if(signature == "clearCanvas()")
        {
            return [](WrenVM *wrenVm) {
                contexts[wrenVm]->clear_canvas();
            };
        }
        else if(signature == "queueKey(_,_,_)")
        {
            return [](WrenVM *wrenVm) {
                contexts[wrenVm]->queue_key((uint8_t) wrenGetSlotDouble(wrenVm, 1),
                                            (uint32_t) wrenGetSlotDouble(wrenVm, 2),
                                            (uint32_t) wrenGetSlotDouble(wrenVm, 3));
            };
        }
    }
    return [](WrenVM*){
        std::cerr << "UNIMPLEMENTED" << std::endl;
    };
}
