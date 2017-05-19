#include "gb_video_renderer.h"

gb_video_renderer::gb_video_renderer(tgb::video_renderer *renderer, gb_video_renderer::render_callback callback, uint32_t bevel) : _renderer{renderer}, _callback{callback}, _bevel{bevel} {
    _doublePixelBuffer = std::unique_ptr<uint32_t[]>(new uint32_t[320 * 288]);
}

void gb_video_renderer::render_screen(uint8_t *buf, uint32_t width, uint32_t height) {
    _renderer->clear(0);

    auto *originalPixels = (uint32_t *) buf;
    auto *doublePixels = (uint32_t *) _doublePixelBuffer.get();

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            auto currentPixel = originalPixels[y * width + x];

            doublePixels[(y * 2) * 320 + x * 2 + 0]   = currentPixel;
            doublePixels[(y * 2) * 320 + x * 2 + 1]   = currentPixel;
            doublePixels[(y * 2) * 320 + x * 2 + 320] = currentPixel;
            doublePixels[(y * 2) * 320 + x * 2 + 321] = currentPixel;
        }
    }

    _renderer->pixels(_doublePixelBuffer.get(), _bevel, _bevel, width * 2, height * 2);
    _callback();
    _renderer->flip();
}
