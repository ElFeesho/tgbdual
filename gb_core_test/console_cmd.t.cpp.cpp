//
// Created by chris on 02/04/17.
//

#include <gtest/gtest.h>
#include <functional>

class ConsoleCmd {
public:
    using ConsoleCallback = std::function<void(std::istream&)>;
    ConsoleCmd(std::string name, ConsoleCallback cb);
    const std::string &name();

    void invoke(std::istream &stream);

private:
    std::string _name;
    ConsoleCallback _cb;
};

ConsoleCmd::ConsoleCmd(std::string name, ConsoleCmd::ConsoleCallback cb) : _name{name}, _cb{cb} {

}

const std::string &ConsoleCmd::name() {
    return _name;
}

void ConsoleCmd::invoke(std::istream &stream) {
    _cb(stream);
}

TEST(can_invoke_cmd_with_no_args, like_void) {
    bool executed = false;
    ConsoleCmd testCommand{"test", [&](std::istream &args){
        executed = true;
    }};

    std::stringstream ss;
    ss << "unused";
    ss << 10;

    testCommand.invoke(ss);

    EXPECT_TRUE(executed);
}

TEST(can_invoke_cmd_with_args, like_two_ints_and_a_string) {
    int one, two;
    std::string three;

    ConsoleCmd testCommand{"test", [&](std::istream &args){
        args >> one;
        args >> two;
        args >> three;
    }};

    std::stringstream ss;
    ss << 10 << " " << 20 << " " << "three";

    testCommand.invoke(ss);

    EXPECT_EQ(10, one);
    EXPECT_EQ(20, two);
    EXPECT_EQ("three", three);
}