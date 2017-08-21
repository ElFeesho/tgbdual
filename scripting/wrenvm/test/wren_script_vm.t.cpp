#include <gtest/gtest.h>
#include <wren_script_vm.h>
#include <map>

class mock_script_services : public script_services {
public:
    void print_string(const std::string &message) override {
        printed_message = message;
    }

    void set_16bit_value(uint32_t address, uint16_t value) override {
        stored16bit.first = address;
        stored16bit.second = value;
    }

    void set_8bit_value(uint32_t address, uint8_t value) override {
        stored8bit.first = address;
        stored8bit.second = value;
    }

    uint8_t read_8bit_value(uint32_t address) override {
        return readable8bitValues[address];
    }

    uint16_t read_16bit_value(uint32_t address) override {
        return readable16bitValues[address];
    }

    void add_image(const std::string &name, int16_t x, int16_t y) override {
        captured_image_file = name;
        captured_image_x = x;
        captured_image_y = y;
    }

    void add_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t stroke, uint32_t fill) override {
        captured_rect = osd_rect{x, y, w, h, stroke, fill};
    }

    void add_text(const std::string &message, int16_t x, int16_t y) override {
        captured_text = message;
        captured_text_x = x;
        captured_text_y = y;
    }

    void queue_key(uint8_t key, uint32_t when, uint32_t duration) override {
        captured_event = input_event{key, when, duration};
    }

    void register_command(const std::string &cmdName, script_command command) override {
        _registeredCommands[cmdName] = command;
    }

    std::string printed_message;
    osd_rect captured_rect;

    std::string captured_text;
    int16_t captured_text_x;
    int16_t captured_text_y;

    std::string captured_image_file;
    int16_t captured_image_x;
    int16_t captured_image_y;

    input_event captured_event{0u, 0u, 0u};

    std::map<std::string, script_context::script_command> _registeredCommands;

    std::map<uint32_t, uint8_t> readable8bitValues;
    std::map<uint32_t, uint16_t> readable16bitValues;

    std::pair<uint32_t, uint8_t> stored8bit;
    std::pair<uint32_t, uint16_t> stored16bit;
};

class WrenVMTest : public ::testing::Test {
public:
    mock_script_services scriptServices;

    wren_script_vm vm{&scriptServices};
};

TEST_F(WrenVMTest, canLoadEmptyScript) {
    EXPECT_NO_FATAL_FAILURE(vm.loadScript(""));
}


TEST_F(WrenVMTest, LuaVM_CanInvokeOnLoadFunction) {
    vm.loadScript("var onLoad = Fn.new {\n"
                          "   GameBoy.print(\"Expected Message\")\n"
                          "}\n");

    EXPECT_EQ("Expected Message", scriptServices.printed_message);
}

TEST_F(WrenVMTest, LuaVM_CanInvokeTickFunction) {
    vm.loadScript(
          "var tick = Fn.new {\n"
          "   GameBoy.print(\"Expected Message\")\n"
          "}\n");

    vm.tick();

    EXPECT_EQ("Expected Message", scriptServices.printed_message);
}

TEST_F(WrenVMTest, LuaVM_CanInvokeActivateFunction) {
    vm.loadScript(
          "var activate = Fn.new {\n"
          "   GameBoy.print(\"Expected Message\")\n"
          "}\n");

    vm.activate();

    EXPECT_EQ("Expected Message", scriptServices.printed_message);
}


TEST_F(WrenVMTest, LuaVM_CanHandleUnhandledCommands) {
    vm.loadScript(
          "var handleCommand = Fn.new { |command, args|\n"
          "   GameBoy.print(\"Got command: %(command) args: %(args[0])\")\n"
          "   return true\n"
          "}\n");

    bool result = vm.handleUnhandledCommand("test_command", {"one"});
    EXPECT_TRUE(result);
    EXPECT_EQ("Got command: test_command args: one", scriptServices.printed_message);
}

TEST_F(WrenVMTest, LuaVM_CanIndicatedInabilityToHandleUnhandledCommands) {
    vm.loadScript(
            "var handleCommand = Fn.new { |command, args|\n"
            "   return false\n"
            "}\n");

    bool result = vm.handleUnhandledCommand("test_command", {"one"});

    EXPECT_FALSE(result);
}

TEST_F(WrenVMTest, WrenVM_CanAddRect) {
    vm.loadScript("GameBoy.addRect(10, 20, 30, 40, 0xff00ff00, 0x00ff00ff)\n");

    EXPECT_EQ(10, scriptServices.captured_rect.x());
    EXPECT_EQ(20, scriptServices.captured_rect.y());
    EXPECT_EQ(30u, scriptServices.captured_rect.w());
    EXPECT_EQ(40u, scriptServices.captured_rect.h());
    EXPECT_EQ(0xff00ff00u, scriptServices.captured_rect.stroke());
    EXPECT_EQ(0x00ff00ffu, scriptServices.captured_rect.fill());
}

TEST_F(WrenVMTest, WrenVM_CanAddText) {
    vm.loadScript("GameBoy.addText(\"Expected Text\", 10, 20)\n");

    EXPECT_EQ("Expected Text", scriptServices.captured_text);
    EXPECT_EQ(10, scriptServices.captured_text_x);
    EXPECT_EQ(20, scriptServices.captured_text_y);
}


TEST_F(WrenVMTest, WrenVM_CanAddImage) {
    vm.loadScript("GameBoy.addImage(\"image_file.png\", 10, 20)\n");

    EXPECT_EQ("image_file.png", scriptServices.captured_image_file);
    EXPECT_EQ(10, scriptServices.captured_image_x);
    EXPECT_EQ(20, scriptServices.captured_image_y);
}


TEST_F(WrenVMTest, WrenVM_CanQueueInputEvents) {
    vm.loadScript("GameBoy.queueKey(0x01, 100, 200)\n");

    EXPECT_EQ(0x01u, scriptServices.captured_event.key);
    EXPECT_EQ(100u, scriptServices.captured_event.when);
    EXPECT_EQ(200u, scriptServices.captured_event.duration);
}

TEST_F(WrenVMTest, WrenVM_CanRead8bitValue) {
    scriptServices.readable8bitValues[0x1234] = 128u;

    vm.loadScript("GameBoy.print(\"Val: %(GameBoy.get8bit(0x1234))\")\n");

    EXPECT_EQ("Val: 128", scriptServices.printed_message);
}

TEST_F(WrenVMTest, WrenVM_CanRead16bitValue) {
    scriptServices.readable16bitValues[0x123] = 32768u;

    vm.loadScript("GameBoy.print(\"Val: %(GameBoy.get16bit(0x123))\")\n");

    EXPECT_EQ("Val: 32768", scriptServices.printed_message);
}

TEST_F(WrenVMTest, WrenVM_CanWrite8bitValue) {
    vm.loadScript("GameBoy.set8bit(0x1234, 128)\n");

    EXPECT_EQ(0x1234u, scriptServices.stored8bit.first);
    EXPECT_EQ(128u, scriptServices.stored8bit.second);
}

TEST_F(WrenVMTest, WrenVM_CanWrite16bitValue) {
    vm.loadScript("GameBoy.set16bit(0x123, 32768)\n");

    EXPECT_EQ(0x123u, scriptServices.stored16bit.first);
    EXPECT_EQ(32768u, scriptServices.stored16bit.second);
}

TEST_F(WrenVMTest, WrenVM_CanRegisterConsoleCommands) {
    vm.loadScript(
            "GameBoy.registerConsoleCommand(\"cfunc\", Fn.new { |args|\n"
                    "    var arg = args[0]\n"
                    "    GameBoy.print(\"Expected Func %(arg)\")\n"
            "})\n");

    EXPECT_TRUE(scriptServices._registeredCommands.find("cfunc") != scriptServices._registeredCommands.end());

    scriptServices._registeredCommands["cfunc"]({"arg"});

    EXPECT_EQ("Expected Func arg", scriptServices.printed_message);
}