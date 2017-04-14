#pragma once

#include <osd_renderer.h>
#include <SDL_video.h>
#include <functional>
#include <map>
#include <vector>

class sdl_osd_renderer : public osd_renderer {
public:
    sdl_osd_renderer(SDL_Surface *screen);

    void render();

    void display_message(const std::string &msg, uint64_t duration) override;

    void add_rect(const osd_rect &rect) override;

    void add_image(const osd_image &image) override;

    void add_text(const std::string &text, int16_t x, int16_t y) override;
private:
    using draw_op = std::function<void()>;
    using pending_osd_message = std::pair<uint64_t, std::string>;
    SDL_Surface *lookupImage(const std::string &image_name);

    SDL_Surface *_screen;

    std::map<std::string, SDL_Surface *> image_cache;
    std::vector<pending_osd_message> osd_messages;
    std::vector<draw_op> pending_operations;

    void drawText(const std::string &message, Sint16 x, Sint16 y) const;

    void drawRect(uint32_t colour, uint32_t fillColour, int16_t x, int16_t y, uint16_t width, uint16_t height) const;

    void renderOSDMessages();


};
