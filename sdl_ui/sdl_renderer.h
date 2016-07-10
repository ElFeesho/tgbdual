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

class sdl_renderer : public renderer {
   public:
    sdl_renderer();
    ~sdl_renderer();

    void render_screen(uint8_t *buf, int width, int height, int depth);
    uint8_t check_pad();
    void set_pad(uint8_t stat);
    void refresh();
    void reset() {}

    uint8_t get_time(int type);
    void set_time(int type, uint8_t dat);

    int get_timer_state();
    void set_timer_state(int timer);

    uint16_t get_sensor(bool x_y);

    sound_renderer *get_sound_renderer() { return snd_render; }

   private:
    void init_sdlvideo();
    void uninit_sdlvideo();

    void init_sdlaudio();

    uint8_t pad_state { 0 };

    int now_sensor_x, now_sensor_y;

    int cur_time;

   private:
    SDL_Surface *dpy;
    SDL_Surface *scr;
};
