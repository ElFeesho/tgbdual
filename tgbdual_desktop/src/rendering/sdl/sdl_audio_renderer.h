#pragma once

#include <audio_renderer.h>

class sdl_audio_renderer : public audio_renderer {
public:
    sdl_audio_renderer();

    void connect_audio_provider(sound_provider *sound_provider) override;
private:
    sound_provider *_sound_provider{nullptr};
};
