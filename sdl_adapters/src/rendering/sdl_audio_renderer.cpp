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
        sdl_audio_renderer *thiz = static_cast<sdl_audio_renderer *>(userData);
        thiz->_fill_cb(stream, len/4);
    };
    wanted.userdata = (void *) this;

    if (SDL_OpenAudio(&wanted, NULL) < 0) {
        std::cerr << "Could not open audio device: " << SDL_GetError() << std::endl;
    }

    SDL_PauseAudio(0);
}

void sdl_audio_renderer::provideFillBufferCallback(tgb::audio_renderer::fill_buffer_cb fill_cb) {
    _fill_cb = fill_cb;
}

