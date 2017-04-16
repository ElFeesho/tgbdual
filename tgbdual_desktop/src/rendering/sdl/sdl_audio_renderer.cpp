#include <SDL.h>
#include <iostream>
#include "sdl_audio_renderer.h"

sdl_audio_renderer::sdl_audio_renderer() {
    SDL_AudioSpec wanted = {0};

    wanted.freq = 44100;
    wanted.format = AUDIO_S16;
    wanted.channels = 2;
    wanted.samples = 4096;
    wanted.callback = [](void *userData, uint8_t *stream, int len) {
        sdl_audio_renderer *renderer = static_cast<sdl_audio_renderer *>(userData);
        sound_provider *provider = renderer->_sound_provider;
        if (provider != nullptr) {
            provider->populate_audio_buffer((short *) stream, len / 4);
        }
    };
    wanted.userdata = (void *) this;

    if (SDL_OpenAudio(&wanted, NULL) < 0) {
        fprintf(stderr, "Could not open audio device: %s\n", SDL_GetError());
    }

    SDL_PauseAudio(0);
}

void sdl_audio_renderer::connect_audio_provider(sound_provider *sound_provider) {
    _sound_provider = sound_provider;
}
