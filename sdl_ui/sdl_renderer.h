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
#include <vector>
#include "../gb_core/renderer.h"

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

    void render_screen(byte *buf, int width, int height, int depth);
    int check_pad();
    void set_pad(int stat);
    void refresh();
    void output_log(const char *mes, ...);
    void reset() {}
    word map_color(word gb_col);
    word unmap_color(word gb_col);
    byte get_time(int type);
    void set_time(int type, byte dat);
    
    void set_key(key_dat *keys);

    void flip();
    void on_move();
    void draw_menu(int n);
    void show_message(const char *message);
    word get_any_key();

    int get_timer_state();
    void set_timer_state(int timer);

    word get_sensor(bool x_y);
    void set_bibrate(bool bibrate);

    void set_filter(col_filter *fil) { m_filter = *fil; };

    void set_save_key(key_dat *key_code) { save_key = *key_code; }
    void set_load_key(key_dat *key_code) { load_key = *key_code; }
    void set_auto_key(key_dat *key_code) { auto_key = *key_code; }

    void set_save_resurve(int slot) { save_resurve = slot; }
    void set_load_resurve(int slot) { load_resurve = slot; }

    bool check_press(key_dat *dat);

    void pause_sound();
    void resume_sound();

    void update_pad();
    void toggle_auto();
    void set_use_ffb(bool use);

   private:
    void init_sdlvideo();
    void uninit_sdlvideo();
    void init_surface();
    void release_surface();


    int width;
    int height;
    int depth;
    int bpp;

    int color_type;
    bool b_640_480;

    int render_pass_type;

    col_filter m_filter;

    uint32_t map_24[0x10000];


    void init_sdlaudio();
    void init_sdlevent();
    void uninit_sdlaudio();
    void uninit_sdlevent();


    int joysticks;
    int pad_state;

    key_dat key_config[8], koro_key_config[4];
    bool b_koro_analog;
    int koro_sence;
    bool b_bibrating;
    bool b_can_use_ffb;
    bool b_use_ffb;

    int now_sensor_x, now_sensor_y;
    
    bool b_auto;

    key_dat save_key, load_key, auto_key;

    int save_resurve;
    int load_resurve;

    int snd_size;

    int cur_time;

   private:
    SDL_Surface *dpy;
    SDL_Surface *scr;
};
