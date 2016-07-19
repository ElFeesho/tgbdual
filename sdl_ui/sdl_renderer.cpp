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

#include <gb.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_image.h>

const uint16_t WIN_MULTIPLIER = 2;

static inline Uint32 getpixel(SDL_Surface *surface, int x, int y) {
	uint8_t *p = (uint8_t *) surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
	return *(Uint16 *) p;
}

sdl_renderer::sdl_renderer() {

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	init_sdlvideo();
	init_sdlaudio();
}

sdl_renderer::~sdl_renderer() {
	SDL_FreeSurface(scr);
}

void sdl_renderer::init_sdlvideo() {
	static const int GB_W = 160;
	static const int GB_H = 144;
	int w = GB_W * WIN_MULTIPLIER;
	int h = GB_H * WIN_MULTIPLIER;
	uint32_t flags = SDL_SWSURFACE;

	dpy = SDL_SetVideoMode(w, h, 16, flags);
	SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, GB_W, GB_H, 16, 0, 0, 0, 0);
	scr = SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);
}

void sdl_renderer::render_screen(uint8_t *buf, int width, int height, int depth) {

	for (int i = 0; i < height; i++) {
		memcpy((void *) (((uint8_t *) scr->pixels) + (scr->pitch * i)), buf + (i * width * depth / 8),
			   (size_t) (width * depth / 8));
	}

	SDL_Rect loc = {0, 0, WIN_MULTIPLIER, WIN_MULTIPLIER};
	for (int y = 0; y < 144; y++) {
		for (int x = 0; x < 160; x++) {
			Uint32 colour = getpixel(scr, x, y);
			loc.x = (Sint16) (x * WIN_MULTIPLIER);
			loc.y = (Sint16) (y * WIN_MULTIPLIER);
			SDL_FillRect(dpy, &loc, colour);
		}
	}

	int16_t msg_number = 0;
	for (auto osd_msg : osd_messages) {
		std::string message = get<1>(osd_msg);

		stringRGBA(dpy, 11, (Sint16) (11 + msg_number * 20), message.c_str(), 0, 0, 0, 128);
		stringRGBA(dpy, 10, (Sint16) (10 + msg_number * 20), message.c_str(), 255, 255, 255, 255);

		msg_number++;
	}

	osd_messages.erase(
			std::remove_if(osd_messages.begin(), osd_messages.end(), [](std::tuple<uint64_t, std::string> msg) -> bool {
				return SDL_GetTicks() > get<0>(msg);
			}), osd_messages.end());


	for (auto rect : rects)
	{
		uint32_t colour = rect.stroke();
		uint8_t a = (uint8_t) ((colour & 0xff000000) >> 24);
		uint8_t b = (uint8_t) ((colour & 0x00ff0000) >> 16);
		uint8_t g = (uint8_t) ((colour & 0x0000ff00) >> 8);
		uint8_t r = (uint8_t) (colour & 0x000000ff);

		uint32_t fillColour = rect.fill();
		uint8_t fillAlpha = (uint8_t) ((fillColour & 0xff000000) >> 24);
		uint8_t fillBlue = (uint8_t) ((fillColour & 0x00ff0000) >> 16);
		uint8_t fillGreen = (uint8_t) ((fillColour & 0x0000ff00) >> 8);
		uint8_t fillRed = (uint8_t) (fillColour & 0x000000ff);

		int16_t x1 = rect.x();
		int16_t x2 = x1 + rect.w();
		int16_t y1 = rect.y();
		int16_t y2 = y1 + rect.h();

		int16_t xpoints[] =  {x1, x2, x2, x1};
		int16_t ypoints[] =  {y1, y1, y2, y2};
		filledPolygonRGBA(dpy, xpoints, ypoints, 4, fillRed, fillGreen, fillBlue, fillAlpha);

		aalineRGBA(dpy, x1, y1, x2, y1, r, g, b, a);
		aalineRGBA(dpy, x1, y2, x2, y2, r, g, b, a);
		aalineRGBA(dpy, x1, y1, x1, y2, r, g, b, a);
		aalineRGBA(dpy, x2, y1, x2, y2, r, g, b, a);
	}

	for(auto image : images)
	{
		SDL_Surface *src = lookupImage(image.name());
		if (src)
		{
			SDL_Rect pos = { image.x(), image.y(), (uint16_t)src->w, (uint16_t)src->h };
			SDL_BlitSurface(src, nullptr, dpy, &pos);
		}
	}

	SDL_UpdateRect(dpy, 0, 0, 0, 0);
}

namespace {
	void fill_audio(void *userData, uint8_t *stream, int len) {
		sdl_renderer *renderer = static_cast<sdl_renderer *>(userData);
		sound_renderer *snd_render = renderer->get_sound_renderer();
		if (snd_render != nullptr) {
			snd_render->render((short *) stream, len / 4);
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
	wanted.userdata = (void *) this;

	if (SDL_OpenAudio(&wanted, NULL) < 0) {
		fprintf(stderr, "Could not open audio device: %s\n", SDL_GetError());
	}

	SDL_PauseAudio(0);
}

void sdl_renderer::display_message(const std::string &msg, uint64_t duration) {
	std::cout << "MSG: " << msg << std::endl;
	osd_messages.emplace_back(std::tuple<uint64_t, std::string>{SDL_GetTicks() + duration, msg});
}

void sdl_renderer::add_rect(const osd_rect &rect) {
	rects.emplace_back(rect);
}

void sdl_renderer::clear_canvas() {
	rects.clear();
	images.clear();
}

void sdl_renderer::add_image(const osd_image &image) {
	images.emplace_back(image);
}

SDL_Surface *sdl_renderer::lookupImage(const string &name) {
	if (image_cache.find(name) == image_cache.end())
	{
		SDL_Surface *icon = IMG_Load(name.c_str());
		if (icon != nullptr)
		{
			image_cache[name] = icon;
		}
	}
	return image_cache[name];
}
