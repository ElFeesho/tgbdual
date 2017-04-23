#include <gtest/gtest.h>
#include <rendering/gb_osd_renderer.h>
#include <emulator_time.h>

class mock_video_renderer : public tgb::video_renderer {
public:
    void fillRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t stroke, uint32_t fill) override {

    }

    void text(const char *text, int32_t x, int32_t y, uint32_t colour) override {
        this->text_text = text;
    }

    void pixels(void *pixels, int32_t x, int32_t y, uint32_t w, uint32_t h) override {

    }

    void image(const char *imgFile, int32_t x, int32_t y) override {

    }

    void clear(uint32_t colour) override {

    }

    void flip() override {

    }

    const char *text_text{nullptr};
};

TEST(GbOsdRenderer, willOnlyDisplayOsdMessageForSpecifiedTimePeriod) {
    mock_video_renderer videoRenderer;
    gb_osd_renderer renderer{&videoRenderer};

    emulator_time::set_time_provider([]{ return 0; });

    renderer.display_message("Expected Message", 1000ul);

    renderer.render();

    EXPECT_STREQ("Expected Message", videoRenderer.text_text);
    videoRenderer.text_text = nullptr;

    emulator_time::set_time_provider([]{ return 1001ul; });

    renderer.render();
    EXPECT_EQ(nullptr, videoRenderer.text_text);
}