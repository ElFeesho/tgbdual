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

    void refresh();
    void reset() {}

    sound_renderer *get_sound_renderer() { return snd_render; }

   private:
    void init_sdlvideo();
    void uninit_sdlvideo();

    void init_sdlaudio();
    
    int now_sensor_x, now_sensor_y;

   private:
    SDL_Surface *dpy;
    SDL_Surface *scr;
};
