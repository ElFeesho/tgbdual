//
// Created by Christopher Sawczuk on 17/04/2017.
//

#include "sfml_audio_renderer.h"

sfml_audio_renderer::sfml_audio_renderer() {

}

void sfml_audio_renderer::provideFillBufferCallback(tgb::audio_renderer::fill_buffer_cb cb) {
    _cb = cb;
    initialize(2, 44100);
    play();
}

bool sfml_audio_renderer::onGetData(sf::SoundStream::Chunk &data) {
    static int sampleCount = 8192;
    static sf::Int16 samples[8192] = {0};
    data.samples = samples;
    data.sampleCount = sampleCount;
    _cb((unsigned char*)data.samples, sampleCount/4);
    return true;
}

void sfml_audio_renderer::onSeek(sf::Time timeOffset) {

}
