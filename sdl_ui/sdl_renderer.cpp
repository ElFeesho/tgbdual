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
    /* Here p is the address to the pixel we want to retrieve */
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
            return 0; /* shouldn't happen, but avoids warnings */
    }
}

sdl_renderer::sdl_renderer() {

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    init_sdlvideo();
    init_sdlaudio();

    cur_time = 0;
    now_sensor_x = now_sensor_y = 2047;
}

sdl_renderer::~sdl_renderer() {
    uninit_sdlvideo();
    uninit_sdlaudio();
}

uint16_t sdl_renderer::map_color(uint16_t gb_col) {

    if (color_type == 0) // ->RRRRRGGG GGxBBBBB に変換 (565) // converted to
        return ((gb_col & 0x1F) << 11) | ((gb_col & 0x3e0) << 1) | ((gb_col & 0x7c00) >> 10) | ((gb_col & 0x8000) >> 10);
    if (color_type == 1) // ->xRRRRRGG GGGBBBBB に変換 (1555) // converted to
        return ((gb_col & 0x1F) << 10) | (gb_col & 0x3e0) | ((gb_col & 0x7c00) >> 10) | (gb_col & 0x8000);
    if (color_type == 2) // ->RRRRRGGG GGBBBBBx に変換 (5551) // converted to
        return ((gb_col & 0x1F) << 11) | ((gb_col & 0x3e0) << 1) | ((gb_col & 0x7c00) >> 9) | (gb_col >> 15);
    else
        return gb_col;
}

uint16_t sdl_renderer::unmap_color(uint16_t gb_col) {
    // xBBBBBGG GGGRRRRR へ変換 // converted to xBBBBBGG GGGRRRRR
    if (color_type == 0) // ->RRRRRGGG GGxBBBBB から変換 (565) // convert from
        return (gb_col >> 11) | ((gb_col & 0x7c0) >> 1) | (gb_col << 10) | ((gb_col & 0x40) << 10);
    if (color_type == 1) // ->xRRRRRGG GGGBBBBB から変換 (1555) // convert from
        return ((gb_col & 0x7c00) >> 10) | (gb_col & 0x3e0) | ((gb_col & 0x1f) << 10) | (gb_col & 0x8000);
    if (color_type == 2) // ->RRRRRGGG GGBBBBBx から変換 (5551) // convert from
        return (gb_col >> 11) | ((gb_col & 0x7c0) >> 1) | ((gb_col & 0x3e) << 9) | (gb_col << 15);
    else
        return gb_col;
}

static uint32_t convert_to_second(struct tm *sys) {
    uint32_t i, ret = 0;
    static int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    for (i = 1; i + 1950 < sys->tm_year; i++)
    {
        if ((i & 3) == 0) 
        {    
            if ((i % 100) == 0) 
            {
                ret += 365 + ((i % 400) == 0 ? 1 : 0);
            }
            else
            {
                ret += 366;
            }
        }
        else 
        {
            ret += 365;
        }
    }

    for (i = 1; i < sys->tm_mon; i++)
    {
        if (i == 2) 
        {
            if ((sys->tm_year & 3) == 0) 
            {
                if ((sys->tm_year % 100) == 0) 
                {
                    if ((sys->tm_year % 400) == 0) 
                    {
                        ret += 29;
                    }
                    else 
                    {
                        ret += 28;
                    }
                }
                else 
                {
                    ret += 29;
                }
            }
            else 
            {
                ret += 28;
            }
        }
        else 
        {
            ret += month_days[i];
        }    
    }

    ret += sys->tm_mday - 1;

    ret *= 24 * 60 * 60;

    ret += sys->tm_hour * 60 * 60;
    ret += sys->tm_min * 60;
    ret += sys->tm_sec;

    return ret;
}

uint8_t sdl_renderer::get_time(int type) {
    struct tm sys;
    time_t t = time(0);
    localtime_r(&t, &sys);

    uint32_t now = convert_to_second(&sys);
    now -= cur_time;

    switch (type) {
        case 8: 
            return (uint8_t)(now % 60);
        case 9: 
            return (uint8_t)((now / 60) % 60);
        case 10:
            return (uint8_t)((now / (60 * 60)) % 24);
        case 11:
            return (uint8_t)((now / (24 * 60 * 60)) & 0xff);
        case 12:
            return (uint8_t)((now / (256 * 24 * 60 * 60)) & 1);
    }
    return 0;
}

void sdl_renderer::set_time(int type, uint8_t dat) {
    struct tm sys;
    time_t t = time(0);
    localtime_r(&t, &sys);

    uint32_t now = convert_to_second(&sys);
    uint32_t adj = now - cur_time;

    switch (type) {
        case 8:
            adj = (adj / 60) * 60 + (dat % 60);
            break;
        case 9:
            adj = (adj / (60 * 60)) * 60 * 60 + (dat % 60) * 60 + (adj % 60);
            break;
        case 10: 
            adj = (adj / (24 * 60 * 60)) * 24 * 60 * 60 + (dat % 24) * 60 * 60 + (adj % (60 * 60));
            break;
        case 11: 
            adj = (adj / (256 * 24 * 60 * 60)) * 256 * 24 * 60 * 60 + (dat * 24 * 60 * 60) + (adj % (24 * 60 * 60));
            break;
        case 12: 
            adj = (dat & 1) * 256 * 24 * 60 * 60 + (adj % (256 * 24 * 60 * 60));
            break;
    }
    cur_time = now - adj;
}

int sdl_renderer::get_timer_state() {
    return cur_time;
}

void sdl_renderer::set_timer_state(int timer) {
    cur_time = timer;
}

uint16_t sdl_renderer::get_sensor(bool x_y) {
    return (x_y ? (now_sensor_x & 0x0fff) : (now_sensor_y & 0x0fff));
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
    Uint64 *sp = (Uint64 *)buf;
    Uint64 *dp = (Uint64 *)scr->pixels;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < (width >> 2); j++) {
            *dp = *sp;
            dp++;
            sp++;
        }
    }

    flip();
}

void sdl_renderer::flip() {

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
        snd_render->render((short *)stream, len / 4);
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

void sdl_renderer::uninit_sdlaudio() {
    SDL_PauseAudio(1);
}

void sdl_renderer::pause_sound() {
    SDL_PauseAudio(1);
}

void sdl_renderer::resume_sound() {
    SDL_PauseAudio(0);
}

int sdl_renderer::check_pad() {
    return pad_state;
}

void sdl_renderer::set_pad(uint8_t stat) {
    pad_state = stat;
}

uint16_t sdl_renderer::get_any_key() {
    return 0;
}

void sdl_renderer::refresh() {
    
}
