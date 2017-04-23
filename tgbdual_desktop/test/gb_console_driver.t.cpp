#include <gtest/gtest.h>
#include <input/gb_console_driver.h>

class mock_renderer : public tgb::video_renderer {
public:
    void fillRect(int32_t, int32_t, uint32_t, uint32_t, uint32_t, uint32_t) override {}

    void text(const char *, int32_t, int32_t, uint32_t) override {}

    void pixels(void *pixels, int32_t, int32_t, uint32_t, uint32_t) override {}

    void image(const char *, int32_t, int32_t) override {}

    void clear(uint32_t) override {}

    void flip() override {}
};

class fake_console_driver : public tgb::console_driver {
public:
    void update(key_down down, key_up up, commandkey_down cmd_down, commandkey_up cmdy_up) override {
        this->down = down;
        this->up = up;
        this->cmd_down = cmd_down;
        this->cmd_up = cmd_up;
    }

    key_down down;
    key_up up;
    commandkey_down cmd_down;
    commandkey_up cmd_up;
};

class GbConsoleDriver : public ::testing::Test {
public:
protected:
    void SetUp() override {
        driver.update();
    }

public:
    mock_renderer renderer;
    fake_console_driver fakeDriver;
    bool shouldHandleUnhandledCommand{false};
    std::string unhandledCommandExecuted;
    std::vector<std::string> unhandledCommandArgs;

    console mock_console{&renderer, 100, 100, [&](std::string &name, std::vector<std::string> &args) {
        unhandledCommandExecuted = name;
        unhandledCommandArgs = args;
        return shouldHandleUnhandledCommand; }, [] { return 0l; }};
    bool consoleClosed = false;
    gb_console_driver driver{mock_console, &fakeDriver, [&] { consoleClosed = true; }};

    void type(const std::string &text) {
        for(char c : text) {
            fakeDriver.down(c);
        }
    }

};

TEST_F(GbConsoleDriver, executingEmptyCommandLineWillNotCrash) {
    EXPECT_NO_FATAL_FAILURE(fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN));
}

TEST_F(GbConsoleDriver, scrollingUpWithNoHistoryWillNotCrash) {
    EXPECT_NO_FATAL_FAILURE(fakeDriver.cmd_down(tgb::console_driver::CommandKey::UP));
}

TEST_F(GbConsoleDriver, scrollingDownWithNoHistoryWillNotCrash) {
    EXPECT_NO_FATAL_FAILURE(fakeDriver.cmd_down(tgb::console_driver::CommandKey::DOWN));
}

TEST_F(GbConsoleDriver, movingLeftWithNoCommandLineWillNotCrash) {
    EXPECT_NO_FATAL_FAILURE(fakeDriver.cmd_down(tgb::console_driver::CommandKey::LEFT));
}

TEST_F(GbConsoleDriver, movingRightWithNoCommandLineWillNotCrash) {
    EXPECT_NO_FATAL_FAILURE(fakeDriver.cmd_down(tgb::console_driver::CommandKey::RIGHT));
}

TEST_F(GbConsoleDriver, tabCompletionOnEmptyCommandLineWillNotCrash) {
    EXPECT_NO_FATAL_FAILURE(fakeDriver.cmd_down(tgb::console_driver::CommandKey::TAB));
}

TEST_F(GbConsoleDriver, consoleCanOpen) {
    mock_console.open();
    ASSERT_TRUE(mock_console.isOpen());
}

TEST_F(GbConsoleDriver, willUseConsoleDriverToDeliverKeyPresses) {
    bool executedCommand = false;
    mock_console.addCommand("a", [&](std::vector<std::string> args) {
        executedCommand = true;
    });

    fakeDriver.down('a');
    fakeDriver.up('a');
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_TRUE(executedCommand);
}

TEST_F(GbConsoleDriver, commandsCanBeRemoved) {
    bool executedCommand = false;
    mock_console.addCommand("a", [&](std::vector<std::string> args) {
        executedCommand = true;
    });

    mock_console.removeCommand("a");

    fakeDriver.down('a');
    fakeDriver.up('a');
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_FALSE(executedCommand);
}

TEST_F(GbConsoleDriver, tabCanBeUsedToCompleteACommand) {
    bool executedCommand = false;
    mock_console.addCommand("abc", [&](std::vector<std::string> args) {
        executedCommand = true;
    });

    fakeDriver.down('a');
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::TAB);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_TRUE(executedCommand);
}

TEST_F(GbConsoleDriver, shortestCommandTabCompletes) {
    mock_console.addCommand("abcd", [](std::vector<std::string> args) {});

    bool executedCommand = false;
    mock_console.addCommand("abc", [&](std::vector<std::string> args) {
        executedCommand = true;
    });

    fakeDriver.down('a');
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::TAB);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_TRUE(executedCommand);
}

TEST_F(GbConsoleDriver, historyCanBeUsedToNavigateToPreviouslyExecutedCommands) {
    bool executedAbcCommand = false;
    bool executedCbaCommand = false;

    mock_console.addCommand("abc", [&](std::vector<std::string> args) {
        executedAbcCommand = true;
    });

    mock_console.addCommand("cba", [&](std::vector<std::string> args) {
        executedCbaCommand = true;
    });

    type("abc");
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);
    EXPECT_TRUE(executedAbcCommand);

    type("cba");
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);
    EXPECT_TRUE(executedCbaCommand);

    executedAbcCommand = false;

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::UP);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::UP);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);
    EXPECT_TRUE(executedAbcCommand);
}

TEST_F(GbConsoleDriver, historyCanBeUsedToNavigateToPreviouslyExecutedCommandsBackDown) {
    bool executedAbcCommand = false;
    bool executedCbaCommand = false;

    mock_console.addCommand("abc", [&](std::vector<std::string> args) {
        executedAbcCommand = true;
    });

    mock_console.addCommand("cba", [&](std::vector<std::string> args) {
        executedCbaCommand = true;
    });

    type("abc");
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);
    EXPECT_TRUE(executedAbcCommand);

    type("cba");
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);
    EXPECT_TRUE(executedCbaCommand);

    executedCbaCommand = false;

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::UP);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::UP);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::DOWN);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);
    EXPECT_TRUE(executedCbaCommand);

    executedAbcCommand = executedCbaCommand = false;

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::UP);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::DOWN);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_FALSE(executedAbcCommand);
    EXPECT_FALSE(executedCbaCommand);
}

TEST_F(GbConsoleDriver, canAlterACommandLine) {

    bool executed = false;

    mock_console.addCommand("ade", [&](std::vector<std::string> args) {
        executed = true;
    });

    type("abc");

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::LEFT);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::BACKSPACE);

    type("de");
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RIGHT);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::BACKSPACE);

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_TRUE(executed);
}

TEST_F(GbConsoleDriver, handledUnhandledCommandsAreAddedToHistory) {

    shouldHandleUnhandledCommand = true;

    type("abc");

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_EQ("abc", unhandledCommandExecuted);

    unhandledCommandExecuted = "";

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::UP);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_EQ("abc", unhandledCommandExecuted);
}

TEST_F(GbConsoleDriver, unhandledUnhandledCommandsAreNotAddedToHistory) {

    shouldHandleUnhandledCommand = false;

    type("abc cba");

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_EQ("abc", unhandledCommandExecuted);

    unhandledCommandExecuted = "";

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::UP);
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_EQ("", unhandledCommandExecuted);
}

TEST_F(GbConsoleDriver, commandsCanHaveArguments) {

    std::vector<std::string> capturedArgs;

    mock_console.addCommand("a", [&](std::vector<std::string> args) {
        capturedArgs = args;
    });

    type("a b");
    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_EQ("b", capturedArgs[0]);
}

TEST_F(GbConsoleDriver, handledUnhandledCommandsCanHaveArguments) {

    shouldHandleUnhandledCommand = true;

    type("abc cba");

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::RETURN);

    EXPECT_EQ("abc", unhandledCommandExecuted);
    EXPECT_EQ("cba", unhandledCommandArgs[0]);
}

TEST_F(GbConsoleDriver, closeConsoleCommandKeyWillExecuteCloseConsoleCallback) {

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::CLOSE_CONSOLE);

    EXPECT_TRUE(consoleClosed);
}
