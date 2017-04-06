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
    static int toInt(const std::string &input);

private:
    std::string _name;
    ConsoleCallback _cb;
};

