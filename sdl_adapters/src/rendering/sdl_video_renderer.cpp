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
// interface video_renderer の SDLを用いた実装
// Using SDL implementation of interface video_renderer

#include "sdl_video_renderer.h"

#include <SDL_gfxPrimitives.h>
#include <SDL_image.h>

#include <string>

static inline uint8_t red(uint32_t colour) { return (uint8_t) (colour & 0x000000ff); }
static inline uint8_t green(uint32_t colour) { return (uint8_t) ((colour & 0x0000ff00) >> 8); }
static inline uint8_t blue(uint32_t colour) { return (uint8_t) ((colour & 0x00ff0000) >> 16); }
static inline uint8_t alpha(uint32_t colour) { return (uint8_t) ((colour & 0xff000000) >> 24); }


sdl_video_renderer::sdl_video_renderer(SDL_Surface *screen) : _screen{screen}, _font{TTF_OpenFont("font.ttf", 10)} {

}

void sdl_video_renderer::fillRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t stroke, uint32_t fill) {
    uint8_t a = alpha(stroke);
    uint8_t b = blue(stroke);
    uint8_t g = green(stroke);
    uint8_t r = red(stroke);

    uint8_t fillAlpha = alpha(fill);
    uint8_t fillBlue = blue(fill);
    uint8_t fillGreen = green(fill);
    uint8_t fillRed = red(fill);

    int16_t x2 = int16_t(x + w);
    int16_t y2 = int16_t(y + h);

    int16_t xpoints[] = {int16_t(x), x2, x2, int16_t(x)};
    int16_t ypoints[] = {int16_t(y), int16_t(y), y2, y2};

    filledPolygonRGBA(_screen, xpoints, ypoints, 4, fillRed, fillGreen, fillBlue, fillAlpha);
    aapolygonRGBA(_screen, xpoints, ypoints, 4, r, g, b, a);
}

void sdl_video_renderer::text(const char *text, int32_t x, int32_t y, uint32_t colour) {
    static SDL_Rect pos;
    uint8_t a = alpha(colour);
    uint8_t b = blue(colour);
    uint8_t g = green(colour);
    uint8_t r = red(colour);

    SDL_Surface *textSurface = TTF_RenderText_Blended(_font, text, SDL_Color{r, g, b, a});
    SDL_Surface *shadowSurface = TTF_RenderText_Blended(_font, text, SDL_Color{0, 0, 0, 220});
    if (textSurface != nullptr) {
        pos.x = (int16_t)x;
        pos.y = (int16_t)y;
        pos.w = (uint16_t)textSurface->w;
        pos.h = (uint16_t)textSurface->h;
        SDL_BlitSurface(shadowSurface, nullptr, _screen, &pos);
        pos.x = int16_t(x-1);
        pos.y = int16_t(y-1);
        SDL_BlitSurface(textSurface, nullptr, _screen, &pos);
        SDL_FreeSurface(textSurface);
        SDL_FreeSurface(shadowSurface);
    }
}

void sdl_video_renderer::pixels(void *pixels, int32_t x, int32_t y, uint32_t w, uint32_t h) {
    SDL_Surface* fromPixels = SDL_CreateRGBSurfaceFrom(pixels, w, h, 16, w*2, 0, 0, 0, 0);
    SDL_Rect loc = {Sint16(x), Sint16(y), Uint16(w), Uint16(h) };

    SDL_BlitSurface(fromPixels, nullptr, _screen, &loc);

    SDL_FreeSurface(fromPixels);
}

void sdl_video_renderer::image(const char *imgFile, int32_t x, int32_t y) {
    SDL_Surface *src = lookupImage(imgFile);
    if (src) {
        SDL_Rect pos = {Sint16(x), Sint16(y), uint16_t(src->w), uint16_t(src->h)};
        SDL_BlitSurface(src, nullptr, _screen, &pos);
    }
}

void sdl_video_renderer::clear(uint32_t colour) {
    SDL_FillRect(_screen, nullptr, colour);
}

void sdl_video_renderer::flip() {
    SDL_Flip(_screen);
}

SDL_Surface *sdl_video_renderer::lookupImage(const std::string &name) {
    if (image_cache.find(name) == image_cache.end()) {
        SDL_Surface *icon = IMG_Load(name.c_str());
        if (icon != nullptr) {
            image_cache.emplace(name, std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)>(icon, SDL_FreeSurface));
        }
    }
    return image_cache.at(name).get();
}

