#pragma once

#include <rendering/audio_renderer.h>

class dummy_audio_renderer : public tgb::audio_renderer {
public:
    void provideFillBufferCallback(fill_buffer_cb cb) override;
};
