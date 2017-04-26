#include <gtest/gtest.h>
#include <rendering/gb_video_renderer.h>

class mock_video_renderer : public tgb::video_renderer {
public:
    void fillRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t stroke, uint32_t fill) override {

    }

    void text(const char *text, int32_t x, int32_t y, uint32_t colour) override {

    }

    void pixels(void *pixels, int32_t x, int32_t y, uint32_t w, uint32_t h) override {
        this->renderedPixels = pixels;
        this->pixels_x = x;
        this->pixels_y = y;
        this->pixels_w = w;
        this->pixels_h = h;
    }

    void image(const char *imgFile, int32_t x, int32_t y) override {

    }

    void clear(uint32_t colour) override {

    }

    void flip() override {

    }

    void *renderedPixels;
    int32_t pixels_x;
    int32_t pixels_y;
    uint32_t pixels_w;
    uint32_t pixels_h;
};


class GbVideoRenderer : public ::testing::Test {
protected:
    void SetUp() override {
        memset(screen,  0xffu, 160*144*2);
    }

public:
    mock_video_renderer mock_renderer;
    gb_video_renderer renderer{&mock_renderer, [&] { called = true; }, 20};

    uint8_t screen[160*144*2];
    bool called{false};
};

TEST_F(GbVideoRenderer, willDoubleSourceInput) {
    renderer.render_screen(screen, 160, 144);

    auto pixels = (uint8_t*)mock_renderer.renderedPixels;

    ASSERT_EQ(20, mock_renderer.pixels_x);
    ASSERT_EQ(20, mock_renderer.pixels_y);
    ASSERT_EQ(320u, mock_renderer.pixels_w);
    ASSERT_EQ(288u, mock_renderer.pixels_h);

    for(int i = 0; i < 320*288*2; i++) {
        ASSERT_EQ(0xffu, pixels[i]);
    }
}

TEST_F(GbVideoRenderer, willExecuteCallback) {
    renderer.render_screen(screen, 160, 144);

    ASSERT_TRUE(called);
}
