#pragma once

#include <audio_renderer.h>
#include "audio_renderer.h"

class gb_audio_renderer : public audio_renderer {
public:
    gb_audio_renderer(tgb::audio_renderer *audioRenderer);

    void connect_audio_provider(sound_provider *sound_provider) override;

private:
    sound_provider *_sound_provider { nullptr };
};
