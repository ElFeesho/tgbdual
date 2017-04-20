#include <gtest/gtest.h>
#include <wren_script_vm.h>
#include <map>

class mock_osd : public osd_renderer {
public:
    void display_message(const std::string &msg, uint64_t duration) override {
        printed_message = msg;
    }

    void add_rect(const osd_rect &rect) override {
        captured_rect = rect;
    }

    void add_image(const osd_image &image) override {
        captured_image_file = image.name();
        captured_image_x = image.x();
        captured_image_y = image.y();
    }

    void add_text(const std::string &text, int16_t x, int16_t y) override {
        captured_text = text;
        captured_text_x = x;
        captured_text_y = y;
    }

    std::string printed_message;
    osd_rect captured_rect;

    std::string captured_text;
    int16_t captured_text_x;
    int16_t captured_text_y;

    std::string captured_image_file;
    int16_t captured_image_x;
    int16_t captured_image_y;
};

class mock_input_queue : public input_queue {
public:
    void queue_key(const input_event &event) override {
        captured_event = event;
    }

    input_event captured_event{0u, 0u, 0u};
};

class mock_memory_bridge : public memory_bridge {
public:
    uint8_t read_8bit(uint32_t addr) override {
        return readable8bitValues[addr];
    }

    void write_8bit(uint32_t addr, uint8_t val) override {
        stored8bit.first = addr;
        stored8bit.second = val;
    }

    uint16_t read_16bit(uint32_t addr) override {
        return readable16bitValues[addr];
    }

    void write_16bit(uint32_t addr, uint16_t val) override {
        stored16bit.first = addr;
        stored16bit.second = val;
    }
    std::map<uint32_t, uint8_t> readable8bitValues;
    std::map<uint32_t, uint16_t> readable16bitValues;

    std::pair<uint32_t, uint8_t> stored8bit;
    std::pair<uint32_t, uint16_t> stored16bit;
};

class WrenVMTest : public ::testing::Test {
public:
    mock_osd mockOsd;
    mock_input_queue mockInputQueue;
    mock_memory_bridge mockMemoryBridge;

    std::map<std::string, script_context::script_command> _registeredCommands;

    script_context _ctx{&mockOsd, &mockInputQueue, &mockMemoryBridge, [&](const std::string &commandName, script_context::script_command command) {
        _registeredCommands[commandName] = command;
    }};

    wren_script_vm vm{_ctx};
};

TEST_F(WrenVMTest, canLoadEmptyScript) {
    EXPECT_NO_FATAL_FAILURE(vm.loadScript(""));
}


TEST_F(WrenVMTest, LuaVM_CanInvokeOnLoadFunction) {
    vm.loadScript("var onLoad = Fn.new {\n"
                          "   GameBoy.print(\"Expected Message\")\n"
                          "}\n");

    EXPECT_EQ("Expected Message", mockOsd.printed_message);
}

TEST_F(WrenVMTest, LuaVM_CanInvokeTickFunction) {
    vm.loadScript(
          "var tick = Fn.new {\n"
          "   GameBoy.print(\"Expected Message\")\n"
          "}\n");

    vm.tick();

    EXPECT_EQ("Expected Message", mockOsd.printed_message);
}

TEST_F(WrenVMTest, LuaVM_CanInvokeActivateFunction) {
    vm.loadScript(
          "var activate = Fn.new {\n"
          "   GameBoy.print(\"Expected Message\")\n"
          "}\n");

    vm.activate();

    EXPECT_EQ("Expected Message", mockOsd.printed_message);
}


TEST_F(WrenVMTest, LuaVM_CanHandleUnhandledCommands) {
    vm.loadScript(
          "var handleCommand = Fn.new { |command, args|\n"
          "   GameBoy.print(\"Got command: %(command) args: %(args[0])\")\n"
          "   return true\n"
          "}\n");

    bool result = vm.handleUnhandledCommand("test_command", {"one"});
    EXPECT_TRUE(result);
    EXPECT_EQ("Got command: test_command args: one", mockOsd.printed_message);
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

    EXPECT_EQ(10, mockOsd.captured_rect.x());
    EXPECT_EQ(20, mockOsd.captured_rect.y());
    EXPECT_EQ(30u, mockOsd.captured_rect.w());
    EXPECT_EQ(40u, mockOsd.captured_rect.h());
    EXPECT_EQ(0xff00ff00u, mockOsd.captured_rect.stroke());
    EXPECT_EQ(0x00ff00ffu, mockOsd.captured_rect.fill());
}

TEST_F(WrenVMTest, WrenVM_CanAddText) {
    vm.loadScript("GameBoy.addText(\"Expected Text\", 10, 20)\n");

    EXPECT_EQ("Expected Text", mockOsd.captured_text);
    EXPECT_EQ(10, mockOsd.captured_text_x);
    EXPECT_EQ(20, mockOsd.captured_text_y);
}


TEST_F(WrenVMTest, WrenVM_CanAddImage) {
    vm.loadScript("GameBoy.addImage(\"image_file.png\", 10, 20)\n");

    EXPECT_EQ("image_file.png", mockOsd.captured_image_file);
    EXPECT_EQ(10, mockOsd.captured_image_x);
    EXPECT_EQ(20, mockOsd.captured_image_y);
}


TEST_F(WrenVMTest, WrenVM_CanQueueInputEvents) {
    vm.loadScript("GameBoy.queueKey(0x01, 100, 200)\n");

    EXPECT_EQ(0x01u, mockInputQueue.captured_event.key);
    EXPECT_EQ(100u, mockInputQueue.captured_event.when);
    EXPECT_EQ(200u, mockInputQueue.captured_event.duration);
}

TEST_F(WrenVMTest, WrenVM_CanRead8bitValue) {
    mockMemoryBridge.readable8bitValues[0x1234] = 128u;

    vm.loadScript("GameBoy.print(\"Val: %(GameBoy.get8bit(0x1234))\")\n");

    EXPECT_EQ("Val: 128", mockOsd.printed_message);
}

TEST_F(WrenVMTest, WrenVM_CanRead16bitValue) {
    mockMemoryBridge.readable16bitValues[0x123] = 32768u;

    vm.loadScript("GameBoy.print(\"Val: %(GameBoy.get16bit(0x123))\")\n");

    EXPECT_EQ("Val: 32768", mockOsd.printed_message);
}

TEST_F(WrenVMTest, WrenVM_CanWrite8bitValue) {
    vm.loadScript("GameBoy.set8bit(0x1234, 128)\n");

    EXPECT_EQ(0x1234u, mockMemoryBridge.stored8bit.first);
    EXPECT_EQ(128u, mockMemoryBridge.stored8bit.second);
}

TEST_F(WrenVMTest, WrenVM_CanWrite16bitValue) {
    vm.loadScript("GameBoy.set16bit(0x123, 32768)\n");

    EXPECT_EQ(0x123u, mockMemoryBridge.stored16bit.first);
    EXPECT_EQ(32768u, mockMemoryBridge.stored16bit.second);
}

TEST_F(WrenVMTest, WrenVM_CanRegisterConsoleCommands) {
    vm.loadScript(
            "GameBoy.registerConsoleCommand(\"cfunc\", Fn.new { |args|\n"
            "    GameBoy.print(\"Expected Func %(args[0])\")\n"
            "})\n");

    EXPECT_TRUE(_registeredCommands.find("cfunc") != _registeredCommands.end());

    _registeredCommands["cfunc"]({"arg"});

    EXPECT_EQ("Expected Func arg", mockOsd.printed_message);
}