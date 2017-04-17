#pragma once

#include <SFML/Audio.hpp>

#include <rendering/audio_renderer.h>

class sfml_audio_renderer : public tgb::audio_renderer, public sf::SoundStream {
public:
    sfml_audio_renderer();
    virtual ~sfml_audio_renderer() {}

    void provideFillBufferCallback(fill_buffer_cb cb) override;

protected:
    bool onGetData(Chunk &data) override;

    void onSeek(sf::Time timeOffset) override;

private:
    fill_buffer_cb _cb;
};
