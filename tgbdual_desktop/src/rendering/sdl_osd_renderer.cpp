//
// Created by Christopher Sawczuk on 14/04/2017.
//

#include <SDL_image.h>
#include <iostream>
#include <SDL_gfxPrimitives.h>
#include "sdl_osd_renderer.h"

static inline uint8_t red(uint32_t colour) { return (uint8_t) (colour & 0x000000ff); }
static inline uint8_t green(uint32_t colour) { return (uint8_t) ((colour & 0x0000ff00) >> 8); }
static inline uint8_t blue(uint32_t colour) { return (uint8_t) ((colour & 0x00ff0000) >> 16); }
static inline uint8_t alpha(uint32_t colour) { return (uint8_t) ((colour & 0xff000000) >> 24); }

sdl_osd_renderer::sdl_osd_renderer(SDL_Surface *screen) : _screen{screen} {

}

void sdl_osd_renderer::display_message(const std::string &msg, uint64_t duration) {
    std::cout << msg << std::endl;
    osd_messages.emplace_back(pending_osd_message{SDL_GetTicks() + duration, msg});
}

void sdl_osd_renderer::add_rect(const osd_rect &rect) {
    pending_operations.push_back([=] {
        drawRect(rect.stroke(), rect.fill(), rect.x(), rect.y(), rect.w(), rect.h());
    });
}

void sdl_osd_renderer::add_image(const osd_image &image) {
    pending_operations.push_back([=] {
        SDL_Surface *src = lookupImage(image.name());
        if (src) {
            SDL_Rect pos = {image.x(), image.y(), uint16_t(src->w), uint16_t(src->h)};
            SDL_BlitSurface(src, nullptr, _screen, &pos);
        }
    });
}

void sdl_osd_renderer::add_text(const std::string &text, int16_t x, int16_t y) {
    pending_operations.push_back([=] {
        drawText(text, x, y);
    });
}

SDL_Surface *sdl_osd_renderer::lookupImage(const std::string &name) {
    if (image_cache.find(name) == image_cache.end()) {
        SDL_Surface *icon = IMG_Load(name.c_str());
        if (icon != nullptr) {
            image_cache[name] = icon;
        }
    }
    return image_cache[name];
}

void sdl_osd_renderer::drawText(const std::string &message, Sint16 x, Sint16 y) const {
    stringRGBA(_screen, x, y, message.c_str(), 0, 0, 0, 128);
    stringRGBA(_screen, (Sint16) (x - 1), (Sint16) (y - 1), message.c_str(), 255, 255, 255, 255);
}

void sdl_osd_renderer::drawRect(uint32_t colour, uint32_t fillColour, int16_t x, int16_t y, uint16_t width, uint16_t height) const {
    uint8_t a = alpha(colour);
    uint8_t b = blue(colour);
    uint8_t g = green(colour);
    uint8_t r = red(colour);

    uint8_t fillAlpha = alpha(fillColour);
    uint8_t fillBlue = blue(fillColour);
    uint8_t fillGreen = green(fillColour);
    uint8_t fillRed = red(fillColour);

    int16_t x2 = x + width;
    int16_t y2 = y + height;

    int16_t xpoints[] = {x, x2, x2, x};
    int16_t ypoints[] = {y, y, y2, y2};

    filledPolygonRGBA(_screen, xpoints, ypoints, 4, fillRed, fillGreen, fillBlue, fillAlpha);
    aapolygonRGBA(_screen, xpoints, ypoints, 4, r, g, b, a);
}

void sdl_osd_renderer::renderOSDMessages() {
    int16_t msg_number = 0;
    for (pending_osd_message &osd_msg : osd_messages) {
        Sint16 y = (Sint16) (11 + msg_number * 20);
        Sint16 x = 11;
        drawText(osd_msg.second, x, y);

        msg_number++;
    }

    osd_messages.erase(
            remove_if(osd_messages.begin(), osd_messages.end(), [](pending_osd_message &msg) { return SDL_GetTicks() > msg.first; }), osd_messages.end());
}

void sdl_osd_renderer::render() {
    for (auto &op : pending_operations) {
        op();
    }

    pending_operations.clear();

    renderOSDMessages();
}

