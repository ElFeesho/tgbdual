#include <gtest/gtest.h>
#include <rendering/gb_osd_renderer.h>
#include <emulator_time.h>

class mock_video_renderer : public tgb::video_renderer {
public:
    void fillRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t stroke, uint32_t fill) override {
        this->rect_x = x;
        this->rect_y = y;
        this->rect_w = w;
        this->rect_h = h;
        this->rect_stroke = stroke;
        this->rect_fill = fill;
    }

    void text(const char *text, int32_t x, int32_t y, uint32_t colour) override {
        this->text_text = text;
        this->text_x = x;
        this->text_y = y;
    }

    void pixels(void *pixels, int32_t x, int32_t y, uint32_t w, uint32_t h) override {

    }

    void image(const char *imgFile, int32_t x, int32_t y) override {
        this->image_name = std::string(imgFile);
        this->image_x = x;
        this->image_y = y;
    }

    void clear(uint32_t colour) override {

    }

    void flip() override {

    }

    const char *text_text{nullptr};
    int32_t text_x{0};
    int32_t text_y{0};
    int32_t rect_x{0};
    int32_t rect_y{0};
    uint32_t rect_w{0};
    uint32_t rect_h{0};
    uint32_t rect_stroke{0};
    uint32_t rect_fill{0};
    std::string image_name{};
    int32_t image_x{0};
    int32_t image_y{0};
};


class GbOsdRenderer : public ::testing::Test {
public:
protected:
    void SetUp() override {
        emulator_time::set_time_provider([]{ return 0; });
    }

public:
    mock_video_renderer videoRenderer;
    gb_osd_renderer renderer{&videoRenderer};
};

TEST_F(GbOsdRenderer, willOnlyDisplayOsdMessageForSpecifiedTimePeriod) {


    renderer.display_message("Expected Message", 1000ul);

    renderer.render();

    EXPECT_STREQ("Expected Message", videoRenderer.text_text);
    videoRenderer.text_text = nullptr;

    emulator_time::set_time_provider([]{ return 1001ul; });

    renderer.render();
    EXPECT_EQ(nullptr, videoRenderer.text_text);
}

TEST_F(GbOsdRenderer, willRenderARectangle) {
    renderer.add_rect(osd_rect{0, 5, 10, 20, 0xff00ff00, 0x00ff00ff});

    renderer.render();

    EXPECT_EQ(0, videoRenderer.rect_x);
    EXPECT_EQ(5, videoRenderer.rect_y);
    EXPECT_EQ(10u, videoRenderer.rect_w);
    EXPECT_EQ(20u, videoRenderer.rect_h);
    EXPECT_EQ(0xff00ff00u, videoRenderer.rect_stroke);
    EXPECT_EQ(0x00ff00ffu, videoRenderer.rect_fill);
}

TEST_F(GbOsdRenderer, willRenderAnImage) {
    renderer.add_image(osd_image{"image.png", 0, 5});

    renderer.render();

    EXPECT_EQ("image.png", videoRenderer.image_name);
    EXPECT_EQ(0, videoRenderer.image_x);
    EXPECT_EQ(5, videoRenderer.image_y);
}

TEST_F(GbOsdRenderer, willRenderText) {
    renderer.add_text("Expected text", 10, 20);

    renderer.render();

    EXPECT_STREQ("Expected text", videoRenderer.text_text);
    EXPECT_EQ(10, videoRenderer.text_x);
    EXPECT_EQ(20, videoRenderer.text_y);

}