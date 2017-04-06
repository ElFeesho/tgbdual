//
// Created by Christopher Sawczuk on 05/04/2017.
//

#pragma once

#include <string>
#include <functional>

class ConsoleCmd {
public:
    using ConsoleCallback = std::function<void(std::vector<std::string>)>;
    ConsoleCmd(std::string name, ConsoleCallback cb);
    const std::string &name();

    void invoke(const std::string &args = "");

    static int fromHex(const std::string &input);
    static int fromDec(const std::string &input);

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

