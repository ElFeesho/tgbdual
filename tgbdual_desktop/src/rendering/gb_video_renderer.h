#pragma once

#include <video_renderer.h>
#include <functional>
#include <rendering/video_renderer.h>

class gb_video_renderer : public video_renderer {
public:
    using render_callback = std::function<void()>;
    gb_video_renderer(tgb::video_renderer *renderer, render_callback callback, uint32_t bevel);
    void render_screen(uint8_t *buf, uint32_t width, uint32_t height, uint32_t depth) override;

private:
    tgb::video_renderer *_renderer;
    render_callback _callback;
    uint32_t _bevel;
    std::unique_ptr<uint8_t> _doublePixelBuffer;
};

