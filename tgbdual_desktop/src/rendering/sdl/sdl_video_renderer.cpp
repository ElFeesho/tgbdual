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

const uint16_t WIN_MULTIPLIER = 2;

static inline Uint32 getpixel(SDL_Surface *surface, int x, int y) {
    return *(Uint32 *)((Uint8*)surface->pixels + (y * surface->pitch + x * surface->format->BytesPerPixel));
}

const int GB_W = 160;
const int GB_H = 144;

sdl_video_renderer::sdl_video_renderer(SDL_Surface *screen, uint16_t bevel, render_callback renderCallback) : _screen{screen}, _bevel{bevel}, _renderCallback{renderCallback} {
    scr = surf_ptr(SDL_CreateRGBSurface(SDL_SWSURFACE, GB_W, GB_H, 16, 0, 0, 0, 0), SDL_FreeSurface);
}

void sdl_video_renderer::render_screen(uint8_t *lcdPixels, int width, int height, int depth) {
    SDL_FillRect(_screen, nullptr, 0);

    memcpy(scr->pixels, lcdPixels, size_t(width*height*3));

    SDL_Rect loc = {0, 0, WIN_MULTIPLIER, WIN_MULTIPLIER};
    for (int y = 0; y < GB_H; y++) {
        for (int x = 0; x < GB_W; x++) {
            loc.x = Sint16(_bevel + x * WIN_MULTIPLIER);
            loc.y = Sint16(_bevel + y * WIN_MULTIPLIER);
            SDL_FillRect(_screen, &loc, getpixel(scr.get(), x, y));
        }
    }

    _renderCallback();
}
