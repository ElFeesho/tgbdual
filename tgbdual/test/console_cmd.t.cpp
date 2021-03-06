#include <gtest/gtest.h>
#include <functional>
#include <console/console_cmd.h>

TEST(can_invoke_cmd_with_no_args, like_void) {
    bool executed = false;
    console_cmd testCommand{[&](std::vector<std::string> args) {
        executed = true;
    }};

    testCommand.invoke();

    EXPECT_TRUE(executed);
}

TEST(can_invoke_cmd_with_args, like_two_ints_and_a_string) {
    int one, two;
    std::string three;

    console_cmd testCommand{[&](std::vector<std::string> args) {
        one = console_cmd::fromDec(args[0]);
        two = console_cmd::fromDec(args[1]);
        two = std::atoi(args[1].c_str());
        three = args[2];
    }};

    testCommand.invoke("10 20 three");

    EXPECT_EQ(10, one);
    EXPECT_EQ(20, two);
    EXPECT_EQ("three", three);
}

TEST(can_invoke_cmd_with_args, like_two_hex_ints) {
    int one, two;

    console_cmd testCommand{[&](std::vector<std::string> args) {
        one = console_cmd::fromHex(args[0]);
        two = console_cmd::fromHex(args[1]);
    }};

    testCommand.invoke("0x10 0x20");

    EXPECT_EQ(0x10, one);
    EXPECT_EQ(0x20, two);
}
