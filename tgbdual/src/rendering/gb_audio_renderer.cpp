#include "gb_audio_renderer.h"

gb_audio_renderer::gb_audio_renderer(tgb::audio_renderer *audioRenderer) {
    audioRenderer->provideFillBufferCallback([&](uint8_t *stream, size_t len) {
        if (_sound_provider != nullptr) {
            _sound_provider->populate_audio_buffer((short*)stream, (int) (len));
        }
    });
}

void gb_audio_renderer::connect_audio_provider(sound_provider *sound_provider) {
    _sound_provider = sound_provider;
}
