/*--------------------------------------------------
   TGB Dual - Gameboy Emulator -
   Copyright (C) 2001  Hii

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//------------------------------------------------------
// interface renderer の SDLを用いた実装
// Using SDL implementation of interface renderer

#include "sdl_renderer.h"

#include <gb.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_image.h>

#include <algorithm>

const uint16_t WIN_MULTIPLIER = 2;

static inline Uint32 getpixel(SDL_Surface *surface, int x, int y) {
    uint8_t *p = (uint8_t *) surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
    return *(Uint16 *) p;
}

sdl_renderer::sdl_renderer() {

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    init_sdlvideo();
    init_sdlaudio();
}

void sdl_renderer::init_sdlvideo() {
    static const int GB_W = 160;
    static const int GB_H = 144;
    int w = GB_W * WIN_MULTIPLIER;
    int h = GB_H * WIN_MULTIPLIER;
    uint32_t flags = SDL_SWSURFACE;

    dpy = surf_ptr(SDL_SetVideoMode(w, h, 16, flags), [](SDL_Surface *) {});
    scr = surf_ptr(SDL_CreateRGBSurface(SDL_SWSURFACE, GB_W, GB_H, 16, 0, 0, 0, 0), SDL_FreeSurface);
}

void sdl_renderer::render_screen(uint8_t *buf, int width, int height, int depth) {

    for (int i = 0; i < height; i++) {
        memcpy((void *) (((uint8_t *) scr->pixels) + (scr->pitch * i)), buf + (i * width * depth / 8),
               (size_t) (width * depth / 8));
    }

    SDL_Rect loc = {0, 0, WIN_MULTIPLIER, WIN_MULTIPLIER};
    for (int y = 0; y < 144; y++) {
        for (int x = 0; x < 160; x++) {
            Uint32 colour = getpixel(scr.get(), x, y);
            loc.x = (Sint16) (x * WIN_MULTIPLIER);
            loc.y = (Sint16) (y * WIN_MULTIPLIER);
            SDL_FillRect(dpy.get(), &loc, colour);
        }
    }

    for (auto &op : pending_operations)
    {
        op();
    }

    pending_operations.clear();

    renderOSDMessages();

    SDL_UpdateRect(dpy.get(), 0, 0, 0, 0);
}

void sdl_renderer::renderOSDMessages() {
    int16_t msg_number = 0;
    for (auto &osd_msg : osd_messages) {
        string message = get<1>(osd_msg);

        Sint16 y = (Sint16) (11 + msg_number * 20);
        Sint16 x = 11;
        drawText(message, x, y);

        msg_number++;
    }

    osd_messages.erase(
            remove_if(osd_messages.begin(), osd_messages.end(), [](tuple<uint64_t, string> msg) -> bool {
                return SDL_GetTicks() > get<0>(msg);
            }), osd_messages.end());
}

void
sdl_renderer::drawRect(uint32_t colour, uint32_t fillColour, int16_t x, int16_t y, uint16_t width, uint16_t height) const {
    uint8_t a = (uint8_t) ((colour & 0xff000000) >> 24);
    uint8_t b = (uint8_t) ((colour & 0x00ff0000) >> 16);
    uint8_t g = (uint8_t) ((colour & 0x0000ff00) >> 8);
    uint8_t r = (uint8_t) (colour & 0x000000ff);

    uint8_t fillAlpha = (uint8_t) ((fillColour & 0xff000000) >> 24);
    uint8_t fillBlue = (uint8_t) ((fillColour & 0x00ff0000) >> 16);
    uint8_t fillGreen = (uint8_t) ((fillColour & 0x0000ff00) >> 8);
    uint8_t fillRed = (uint8_t) (fillColour & 0x000000ff);

    int16_t x1 = x;
    int16_t x2 = x1 + width;
    int16_t y1 = y;
    int16_t y2 = y1 + height;

    int16_t xpoints[] = {x1, x2, x2, x1};
    int16_t ypoints[] = {y1, y1, y2, y2};
    filledPolygonRGBA(dpy.get(), xpoints, ypoints, 4, fillRed, fillGreen, fillBlue, fillAlpha);
    aapolygonRGBA(dpy.get(), xpoints, ypoints, 4, r, g, b, a);
}

void sdl_renderer::drawText(const string &message, Sint16 x, Sint16 y) const {
    stringRGBA(dpy.get(), x, y, message.c_str(), 0, 0, 0, 128);
    stringRGBA(dpy.get(), (Sint16) (x - 1), (Sint16) (y - 1), message.c_str(), 255, 255, 255, 255);
}

namespace {
    void fill_audio(void *userData, uint8_t *stream, int len) {
        sdl_renderer *renderer = static_cast<sdl_renderer *>(userData);
        sound_renderer *snd_render = renderer->get_sound_renderer();
        if (snd_render != nullptr) {
            snd_render->render((short *) stream, len / 4);
        }
    }
}

void sdl_renderer::init_sdlaudio() {
    SDL_AudioSpec wanted = { 0 };

    wanted.freq = 44100;
    wanted.format = AUDIO_S16;
    wanted.channels = 2;
    wanted.samples = 4096;
    wanted.callback = fill_audio;
    wanted.userdata = (void *) this;

    if (SDL_OpenAudio(&wanted, NULL) < 0) {
        fprintf(stderr, "Could not open audio device: %s\n", SDL_GetError());
    }

    SDL_PauseAudio(0);
}

void sdl_renderer::display_message(const std::string &msg, uint64_t duration) {
    std::cout << "MSG: " << msg << std::endl;
    osd_messages.emplace_back(std::tuple<uint64_t, std::string>{SDL_GetTicks() + duration, msg});
}

void sdl_renderer::add_rect(const osd_rect &rect) {
    pending_operations.push_back([=]{
        uint32_t colour = rect.stroke();
        uint32_t fillColour = rect.fill();
        int16_t x = rect.x();
        int16_t y = rect.y();
        uint16_t width = rect.w();
        uint16_t height = rect.h();

        drawRect(colour, fillColour, x, y, width, height);
    });
}

void sdl_renderer::clear_canvas() {
}

void sdl_renderer::add_image(const osd_image &image) {
    pending_operations.push_back([=]{
        SDL_Surface *src = lookupImage(image.name());
        if (src) {
            SDL_Rect pos = {image.x(), image.y(), (uint16_t) src->w, (uint16_t) src->h};
            SDL_BlitSurface(src, nullptr, dpy.get(), &pos);
        }
    });
}

SDL_Surface *sdl_renderer::lookupImage(const string &name) {
    if (image_cache.find(name) == image_cache.end()) {
        SDL_Surface *icon = IMG_Load(name.c_str());
        if (icon != nullptr) {
            image_cache[name] = icon;
        }
    }
    return image_cache[name];
}

sound_renderer *sdl_renderer::get_sound_renderer() { return snd_render; }

void sdl_renderer::add_text(const string &text, int16_t x, int16_t y) {
    pending_operations.push_back([=]{
        drawText(text, x, y);
    });
}