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
    console mock_console{&renderer, 100, 100, [](std::string &name, std::vector<std::string> &args) { return false; }, [] { return 0l; }};
    bool consoleClosed = false;
    gb_console_driver driver{mock_console, &fakeDriver, [&] { consoleClosed = true; }};
};

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

TEST_F(GbConsoleDriver, closeConsoleCommandKeyWillExecuteCloseConsoleCallback) {

    fakeDriver.cmd_down(tgb::console_driver::CommandKey::CLOSE_CONSOLE);

    EXPECT_TRUE(consoleClosed);
}
