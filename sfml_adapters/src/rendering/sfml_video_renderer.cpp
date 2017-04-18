//
// Created by Christopher Sawczuk on 17/04/2017.
//

#include <emulator_time.h>
#include <SFML/Window.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <map>
#include <SFML/Graphics/Text.hpp>
#include "sfml_video_renderer.h"

static inline uint32_t swapEndianness(uint32_t input) {
    return ((input >> 24) & 0xff) |
           ((input << 8) & 0xff0000) |
           ((input >> 8) & 0xff00) |
           ((input << 24) & 0xff000000);
}

sfml_video_renderer::sfml_video_renderer(sf::RenderWindow &window, sf::Font &font) : _window{window}, _font{font} {

}

void sfml_video_renderer::fillRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t stroke, uint32_t fill) {
    sf::RectangleShape r{{(float)w, (float)h}};
    r.move(x, y);
    r.setFillColor(sf::Color{swapEndianness(fill)});
    r.setOutlineColor(sf::Color{swapEndianness(stroke)});
    r.setOutlineThickness(1.0f);
    _window.draw(r);
}

void sfml_video_renderer::text(const char *text, int32_t x, int32_t y, uint32_t colour) {
    sf::Text t{text, _font, 10};
    t.move(x, y);
    t.setFillColor(sf::Color{0x00000088});
    _window.draw(t);
    t.move(-1, -1);
    t.setFillColor(sf::Color{swapEndianness(colour)});
    _window.draw(t);
}

void sfml_video_renderer::pixels(void *pixels, int32_t x, int32_t y, uint32_t w, uint32_t h) {
    sf::Texture m{};
    m.create(w, h);
    sf::Uint16* pixels_16bit = (sf::Uint16 *) pixels;
    sf::Uint8* pixels_32bit = new sf::Uint8[w * h * 4];
    for(unsigned int i = 0; i < h; i++)
    {
        for(unsigned int j = 0; j < w; j++)
        {
            unsigned short cpixel = pixels_16bit[w*i + j];
            unsigned char red = (unsigned char) ((cpixel & 0xf800) >> 11);
            unsigned char green = (unsigned char) ((cpixel & 0x07e0) >> 5);
            unsigned char blue = (unsigned char) (cpixel & 0x001f);

            pixels_32bit[(w*i*4)+j*4 + 0] = (sf::Uint8) ((red / 31.f) * 255.f);
            pixels_32bit[(w*i*4)+j*4 + 1] = (sf::Uint8) ((green / 63.f) * 255.f);
            pixels_32bit[(w*i*4)+j*4 + 2] = (sf::Uint8) ((blue / 31.f) * 255.f);
            pixels_32bit[(w*i*4)+j*4 + 3] = 255;
        }
    }

    m.update(pixels_32bit);

    sf::Sprite sprite{m};
    sprite.move(x, y);

    _window.draw(sprite);
}

void sfml_video_renderer::image(const char *imgFile, int32_t x, int32_t y) {
    static std::map<std::string, sf::Texture> imageMap;
    if (imageMap.find(imgFile) == imageMap.end())
    {
        imageMap[imgFile].loadFromFile(imgFile);
    }
    sf::Sprite s{imageMap[imgFile]};
    s.move(x, y);
    _window.draw(s);
}

void sfml_video_renderer::clear(uint32_t colour) {
    _window.clear(sf::Color(swapEndianness(colour)));
}

void sfml_video_renderer::flip() {
    _window.display();
}
