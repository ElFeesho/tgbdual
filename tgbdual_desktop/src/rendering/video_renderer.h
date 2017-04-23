#pragma once

#include <cstdint>

namespace tgb {
    class video_renderer {
    public:
        virtual ~video_renderer()= default;

        virtual void fillRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t stroke, uint32_t fill) = 0;
        virtual void text(const char *text, int32_t x, int32_t y, uint32_t colour) = 0;
        virtual void pixels(void* pixels, int32_t x, int32_t y, uint32_t w, uint32_t h) = 0;
        virtual void image(const char *imgFile, int32_t x, int32_t y) = 0;

        virtual void clear(uint32_t colour) = 0;

        virtual void flip() = 0;
    };
}
