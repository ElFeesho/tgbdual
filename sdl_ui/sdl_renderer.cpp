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

#include <stdarg.h>
#include <stdio.h>

#include <gb.h>

#define WIN_MULTIPLIER 2

static inline Uint32 getpixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    uint8_t *p = (uint8_t *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            return *p;
            break;

        case 2:
            return *(Uint16 *)p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(Uint32 *)p;
            break;

        default:
            return 0;
    }
}

sdl_renderer::sdl_renderer() {

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    init_sdlvideo();
    init_sdlaudio();

    now_sensor_x = now_sensor_y = 2047;
}

sdl_renderer::~sdl_renderer() {
    uninit_sdlvideo();
}

void sdl_renderer::init_sdlvideo() {
    static const int GB_W = 160;
    static const int GB_H = 144;
    int w = GB_W * WIN_MULTIPLIER;
    int h = GB_H * WIN_MULTIPLIER;
    Uint32 flags = SDL_SWSURFACE;

    if (w != GB_W || h != GB_H) {
        dpy = SDL_SetVideoMode(w, h, 16, flags);
        SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, GB_W, GB_H, 16, 0, 0, 0, 0);
        scr = SDL_DisplayFormat(tmp);
        SDL_FreeSurface(tmp);
    } else {
        dpy = 0;
        scr = SDL_SetVideoMode(GB_W, GB_H, 16, flags);
    }
}

void sdl_renderer::uninit_sdlvideo() {
    if (dpy) {
        SDL_FreeSurface(scr);
    }
}

void sdl_renderer::render_screen(uint8_t *buf, int width, int height, int depth) {
    
    for(int i = 0; i < height; i++)
    {
        memcpy(scr->pixels+(scr->pitch*i), buf+(i*width*depth/8), width*depth/8);
    }

    SDL_Rect loc = {0, 0, WIN_MULTIPLIER, WIN_MULTIPLIER};
    if (dpy) {
        for (int y = 0; y < 144; y++) {
            for (int x = 0; x < 160; x++) {
                Uint32 colour = getpixel(scr, x, y);
                loc.x = x * WIN_MULTIPLIER;
                loc.y = y * WIN_MULTIPLIER;
                SDL_FillRect(dpy, &loc, colour);
            }
        }
        SDL_UpdateRect(dpy, 0, 0, 0, 0);
    } else {
        SDL_UpdateRect(scr, 0, 0, 0, 0);
    }
}

namespace {
    void fill_audio(void *userData, uint8_t *stream, int len) {
        sdl_renderer *renderer = static_cast<sdl_renderer *>(userData);
        sound_renderer *snd_render = renderer->get_sound_renderer();
        if (snd_render != nullptr)
        {
            snd_render->render((short *)stream, len / 4);
        }
    }
}

void sdl_renderer::init_sdlaudio() {
    SDL_AudioSpec wanted;

    wanted.freq = 44100;
    wanted.format = AUDIO_S16;
    wanted.channels = 2; 
    wanted.samples = 4096;
    wanted.callback = fill_audio;
    wanted.userdata = (void *)this;

    if (SDL_OpenAudio(&wanted, NULL) < 0) {
        fprintf(stderr, "Could not open audio device: %s\n", SDL_GetError());
    }

    SDL_PauseAudio(0);
}

void sdl_renderer::refresh() {
    
}
