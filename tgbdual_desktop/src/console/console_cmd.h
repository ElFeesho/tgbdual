#pragma once

#include <string>
#include <functional>

class console_cmd {
public:
    using ConsoleCallback = std::function<void(std::vector<std::string>)>;
    console_cmd(std::string name, ConsoleCallback cb);
    const std::string &name();

    void invoke(const std::string &args = "");

    static int fromHex(const std::string &input);
    static int fromDec(const std::string &input);
    static std::vector<std::string> splitArguments(const std::string &args);

    template<typename INTTYPE>
    static INTTYPE toInt(const std::string &input) {
        if (input.find("0x") == 0) {
            return (INTTYPE)fromHex(input);
        }
        return (INTTYPE)fromDec(input);
    }


private:
    std::string _name;
    ConsoleCallback _cb;
};

