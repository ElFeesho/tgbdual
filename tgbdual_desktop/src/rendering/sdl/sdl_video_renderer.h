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
#include <video_renderer.h>

#include <SDL.h>
#include "osd_renderer.h"
#include <vector>
#include <tuple>
#include <map>
#include <memory>

class sdl_video_renderer : public video_renderer {
public:
    using render_callback = std::function<void()>;
    using surf_ptr = std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)>;

    sdl_video_renderer(SDL_Surface *screen, uint16_t bevel, render_callback renderCallback);

    void render_screen(uint8_t *lcdPixels, int width, int height, int depth) override;

private:
    SDL_Surface *_screen;
    uint16_t _bevel;
    surf_ptr scr{nullptr, SDL_FreeSurface};

    render_callback _renderCallback;
};
