//
// Created by chris on 02/04/17.
//

#include <gtest/gtest.h>
#include <functional>

class ConsoleCmd {
public:
    using ConsoleCallback = std::function<void(std::vector<std::string>)>;
    ConsoleCmd(std::string name, ConsoleCallback cb);
    const std::string &name();

    void invoke(const std::string &args = "");

private:
    std::string _name;
    ConsoleCallback _cb;
};

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

TEST(can_invoke_cmd_with_no_args, like_void) {
    bool executed = false;
    ConsoleCmd testCommand{"test", [&](std::vector<std::string> args){
        executed = true;
    }};

    testCommand.invoke();

    EXPECT_TRUE(executed);
}

TEST(can_invoke_cmd_with_args, like_two_ints_and_a_string) {
    int one, two;
    std::string three;

    ConsoleCmd testCommand{"test", [&](std::vector<std::string> args){
        one = std::atoi(args[0].c_str());
        two = std::atoi(args[1].c_str());
        three = args[2];
    }};

    testCommand.invoke("10 20 three");

    EXPECT_EQ(10, one);
    EXPECT_EQ(20, two);
    EXPECT_EQ("three", three);
}
