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

#include <stdio.h>
#include <renderer.h>

#include <SDL.h>

using namespace std;

///#define DIRECTDRAW_VERSION 0x0300
///#define DIRECTSOUND_VERSION 0x0300
///#define DIRECTINPUT_VERSION 0x0500

///#include "wavwrite.h"

///#include "sock.h"

#define DI_KEYBOARD 1
#define DI_MOUSE_X 2
#define DI_MOUSE_Y 3
#define DI_MOUSE 4
#define DI_PAD_X 5
#define DI_PAD_Y 6
#define DI_PAD 7
#define NEXT_PAD 3

struct key_dat {
    int device_type;
    int key_code;
};

struct mov_key {
    int frame;
    int key_code;
};

struct col_filter {
    int r_def, g_def, b_def;
    int r_div, g_div, b_div;
    int r_r, r_g, r_b;
    int g_r, g_g, g_b;
    int b_r, b_g, b_b;
};

class sdl_renderer : public renderer {
   public:
    sdl_renderer();
    ~sdl_renderer();

    void render_screen(uint8_t *buf, int width, int height, int depth);
    int check_pad();
    void set_pad(int stat);
    void refresh();
    void reset() {}
    uint16_t map_color(uint16_t gb_col);
    uint16_t unmap_color(uint16_t gb_col);
    uint8_t get_time(int type);
    void set_time(int type, uint8_t dat);

    void set_key(key_dat *keys);

    void flip();
    void on_move();
    void draw_menu(int n);
    uint16_t get_any_key();

    int get_timer_state();
    void set_timer_state(int timer);

    uint16_t get_sensor(bool x_y);

    void set_filter(col_filter *fil) { };

    bool check_press(key_dat *dat);

    void pause_sound();
    void resume_sound();

    void update_pad();
    void set_use_ffb(bool use);

    sound_renderer *get_sound_renderer() { return snd_render; }

   private:
    void init_sdlvideo();
    void uninit_sdlvideo();

    void init_sdlaudio();
    void uninit_sdlaudio();


    int width;
    int height;
    int depth;
    int bpp;

    int color_type;

    int render_pass_type;

    int pad_state { 0 };

    bool b_can_use_ffb;
    bool b_use_ffb;

    int now_sensor_x, now_sensor_y;

    int cur_time;

   private:
    SDL_Surface *dpy;
    SDL_Surface *scr;
};
