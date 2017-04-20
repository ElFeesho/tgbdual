#include <gtest/gtest.h>

#include <map>

#include <script_context.h>

class fake_memory_bridge : public memory_bridge {
public:
    uint8_t read_8bit(uint32_t addr) override {
        return _8bitValues[addr];
    }

    void write_8bit(uint32_t addr, uint8_t val) override {
        _8bitValues[addr] = val;
    }

    uint16_t read_16bit(uint32_t addr) override {
        return _16bitValues[addr];
    }

    void write_16bit(uint32_t addr, uint16_t val) override {
        _16bitValues[addr] = val;
    }

    std::map<uint32_t, uint8_t> _8bitValues;
    std::map<uint32_t, uint16_t> _16bitValues;
};

class capturing_input_queue : public input_queue {
public:
    void queue_key(const input_event &event) override {
        captured_key = event.key;
        when = event.when;
        duration = event.duration;
    }

    uint8_t captured_key;
    uint32_t when;
    uint32_t duration;
};

class capturing_osd_renderer : public osd_renderer {
public:
    void display_message(const std::string &msg, uint64_t duration) override {
        printed_string = msg;
    }

    void add_rect(const osd_rect &rect) override {
        added_rect_x = rect.x();
        added_rect_y = rect.y();
        added_rect_w = rect.w();
        added_rect_h = rect.h();
        added_rect_fill = rect.fill();
        added_rect_stroke = rect.stroke();
    }

    void add_image(const osd_image &image) override {
        added_image = image.name();
        added_image_x = image.x();
        added_image_y = image.y();
    }

    void add_text(const std::string &text, int16_t x, int16_t y) override {
        added_text_text = text;
        added_text_x = x;
        added_text_y = y;
    }

    std::string printed_string;
    std::string added_image;
    int16_t added_image_x;
    int16_t added_image_y;

    int16_t added_rect_x;
    int16_t added_rect_y;
    uint16_t added_rect_w;
    uint16_t added_rect_h;
    uint32_t added_rect_stroke;
    uint32_t added_rect_fill;

    std::string added_text_text;
    int16_t added_text_x;
    int16_t added_text_y;

};

class script_context_fixture : public ::testing::Test {
public:
    void SetUp() {
    }

    capturing_osd_renderer capturingOsdRenderer;
    capturing_input_queue capturingInputQueue;
    fake_memory_bridge fakeMemoryBridge;
    std::map<std::string, script_context::script_command> registered_commands;

    std::function<void(const std::string &, script_context::script_command)> command_registrar {
        [&](const std::string &name, script_context::script_command command){
            registered_commands[name] = command;
        }
    };



    script_context ctx{&capturingOsdRenderer, &capturingInputQueue, &fakeMemoryBridge, command_registrar};
};

TEST_F(script_context_fixture, CanUseAnOsdRendererToPrintAMessage) {
    ctx.print_string("Expected Message");

    EXPECT_EQ("Expected Message", capturingOsdRenderer.printed_string);
}

TEST_F(script_context_fixture, CanUseAnOsdRendererToDrawAnImage) {
    ctx.add_image("expected_image.png", 10, 20);

    EXPECT_EQ("expected_image.png", capturingOsdRenderer.added_image);
    EXPECT_EQ(10, capturingOsdRenderer.added_image_x);
    EXPECT_EQ(20, capturingOsdRenderer.added_image_y);
}

TEST_F(script_context_fixture, CanUseAnOsdRendererToDrawAnRect) {
    ctx.add_rect(10, 20, 30, 40, 0xff00ff00, 0x00ff00ff);

    EXPECT_EQ(10, capturingOsdRenderer.added_rect_x);
    EXPECT_EQ(20, capturingOsdRenderer.added_rect_y);
    EXPECT_EQ(30u, capturingOsdRenderer.added_rect_w);
    EXPECT_EQ(40u, capturingOsdRenderer.added_rect_h);
    EXPECT_EQ(0xff00ff00u, capturingOsdRenderer.added_rect_stroke);
    EXPECT_EQ(0x00ff00ffu, capturingOsdRenderer.added_rect_fill);
}

TEST_F(script_context_fixture, CanUseAnOsdRendererToDrawText) {
    ctx.add_text("expected text", 10, 20);

    EXPECT_EQ("expected text", capturingOsdRenderer.added_text_text);
    EXPECT_EQ(10, capturingOsdRenderer.added_text_x);
    EXPECT_EQ(20, capturingOsdRenderer.added_text_y);
}

TEST_F(script_context_fixture, CanRegisterConsoleCommands) {
    std::vector<std::string> capturedArgs;
    ctx.register_command("test_command", [&](std::vector<std::string> args) {
        capturedArgs = args;
    });

    registered_commands["test_command"]({"arg"});

    EXPECT_EQ("arg", capturedArgs[0]);
}

TEST_F(script_context_fixture, CanQueueKey) {
    ctx.queue_key(1u, 10u, 30u);

    EXPECT_EQ(1u, capturingInputQueue.captured_key);
    EXPECT_EQ(10u, capturingInputQueue.when);
    EXPECT_EQ(30u, capturingInputQueue.duration);
}

TEST_F(script_context_fixture, can_use_memory_bridge_to_read_8bit_value) {
    fakeMemoryBridge._8bitValues[0x1234u] = 0xa3u;

    EXPECT_EQ(0xa3u, ctx.read_8bit_value(0x1234u));
}

TEST_F(script_context_fixture, can_write_8bit_value) {
    ctx.set_8bit_value(0x1234u, 0xa3u);

    EXPECT_EQ(0xa3u, fakeMemoryBridge._8bitValues[0x1234u]);
}

TEST_F(script_context_fixture, can_use_memory_bridge_to_read_16bit_value) {
    fakeMemoryBridge._16bitValues[0x1234u] = 0xa3a3u;

    EXPECT_EQ(0xa3a3u, ctx.read_16bit_value(0x1234u));
}

TEST_F(script_context_fixture, can_write_16bit_value) {
    ctx.set_16bit_value(0x1234u, 0xa3a3u);

    EXPECT_EQ(0xa3a3u, fakeMemoryBridge._16bitValues[0x1234u]);
}


