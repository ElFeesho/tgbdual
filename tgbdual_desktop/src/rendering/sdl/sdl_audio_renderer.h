#pragma once

#include <rendering/audio_renderer.h>

class sdl_audio_renderer : public tgb::audio_renderer {
public:
    sdl_audio_renderer();

    void provideFillBufferCallback(tgb::audio_renderer::fill_buffer_cb) override;

private:
    tgb::audio_renderer::fill_buffer_cb _fill_cb;
};
