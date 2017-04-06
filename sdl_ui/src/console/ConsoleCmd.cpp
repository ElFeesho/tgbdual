//
// Created by Christopher Sawczuk on 05/04/2017.
//

#include <sstream>
#include <vector>
#include "ConsoleCmd.h"

ConsoleCmd::ConsoleCmd(std::string name, ConsoleCmd::ConsoleCallback cb) : _name{name}, _cb{cb} {

}

const std::string &ConsoleCmd::name() {
    return _name;
}

std::string stringUntil(const std::string &input, char delim) {
    unsigned long delimPosition = input.find(delim);
    if (delimPosition == std::string::npos)
    {
        return input;
    }

    return input.substr(0, delimPosition);
}

void ConsoleCmd::invoke(const std::string &args) {
    std::vector<std::string> components;
    std::string arg = args;
    while(arg.length()>0)
    {
        std::string comp = stringUntil(arg, ' ');
        arg = arg.substr(comp.length());
        if (arg.find(' ') == 0)
        {
            arg = arg.substr(1);
        }
        components.push_back(comp);
    }

    _cb(components);
}

int ConsoleCmd::fromHex(const std::string &input) {
    int output;
    std::stringstream s;
    s << std::hex << input;
    s >> output;
    return output;
}

int ConsoleCmd::fromDec(const std::string &input) {
    int output;
    std::stringstream s;
    s << input;
    s >> output;
    return output;
}

