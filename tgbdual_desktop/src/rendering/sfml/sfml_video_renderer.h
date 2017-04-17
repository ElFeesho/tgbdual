#pragma once

#include <rendering/video_renderer.h>
#include <SFML/Graphics/RenderWindow.hpp>

class sfml_video_renderer : public tgb::video_renderer {
public:
    sfml_video_renderer(sf::RenderWindow &window);

    void fillRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t stroke, uint32_t fill) override;

    void text(const char *text, int32_t x, int32_t y, uint32_t colour) override;

    void pixels(void *pixels, int32_t x, int32_t y, uint32_t w, uint32_t h) override;

    void image(const char *imgFile, int32_t x, int32_t y) override;

    void clear(uint32_t colour) override;

    void flip() override;

private:
    sf::Clock _clock;
    sf::RenderWindow &_window;
};

