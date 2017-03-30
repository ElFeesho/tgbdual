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
#include "osd_renderer.h"
#include <vector>
#include <tuple>
#include <map>
#include <memory>

using namespace std;

class sdl_renderer : public renderer, public osd_renderer {
public:
	using surf_ptr = std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)>;
	sdl_renderer();

	void render_screen(uint8_t *buf, int width, int height, int depth) override;

	sound_renderer *get_sound_renderer();

	virtual void display_message(const std::string &msg, uint64_t duration) override;

	virtual void add_rect(const osd_rect &rect) override;

	virtual void clear_canvas() override;

	virtual void add_image(const osd_image &image) override;

private:
	void init_sdlvideo();

	void init_sdlaudio();

	SDL_Surface *lookupImage(const std::string &image_name);

	surf_ptr dpy { nullptr, SDL_FreeSurface };
	surf_ptr scr { nullptr, SDL_FreeSurface };

	std::vector<std::tuple<uint64_t, std::string>> osd_messages;
	std::vector<osd_rect> rects;
	std::vector<osd_image> images;

	std::map<std::string, SDL_Surface*> image_cache;
};
