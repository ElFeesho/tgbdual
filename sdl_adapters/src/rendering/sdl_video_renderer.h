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
#include <rendering/video_renderer.h>

#include <SDL.h>
#include <vector>
#include <tuple>
#include <map>
#include <memory>
#include <rendering/video_renderer.h>
#include <SDL_ttf.h>

class sdl_video_renderer : public tgb::video_renderer {
public:
    using surf_ptr = std::unique_ptr<SDL_Surface, void (*)(SDL_Surface *)>;

    sdl_video_renderer(SDL_Surface *screen);

    void fillRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t stroke, uint32_t fill) override;

    void text(const char *text, int32_t x, int32_t y, uint32_t colour) override;

    void pixels(void *pixels, int32_t x, int32_t y, uint32_t w, uint32_t h) override;

    void image(const char *imgFile, int32_t x, int32_t y) override;

    void clear(uint32_t colour) override;

    void flip() override;

private:
    SDL_Surface *lookupImage(const std::string &name);

    SDL_Surface *_screen;
    std::unique_ptr<TTF_Font, void(*)(TTF_Font*)> _font;
    std::map<std::string, std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)>> image_cache;
};
