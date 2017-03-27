//
// Created by Christopher Sawczuk on 15/07/2016.
//

#pragma once

#include <cstdint>
#include <string>

class osd_rect
{
public:
	osd_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t strokeColour, uint32_t fillColour) : _x(x), _y{y}, _w{w}, _h{h}, _strokeColour{strokeColour}, _fillColour{fillColour} {}

	int16_t x() { return _x; }
	int16_t y() { return _y; }
	int16_t w() { return _w; }
	int16_t h() { return _h; }
	uint32_t stroke() { return _strokeColour; }
	uint32_t fill() { return _fillColour; }

private:
	int16_t _x;
	int16_t _y;
	int16_t _w;
	int16_t _h;
	uint32_t _strokeColour;
	uint32_t _fillColour;
};

class osd_image {
public:
	osd_image(const std::string &name, int16_t x, int16_t y) : _name{name}, _x{x}, _y{y} {}
	std::string name() { return _name; }
	int16_t x() { return _x; }
	int16_t y() { return _y; }
private:
	std::string _name;
	int16_t _x;
	int16_t _y;
};

class osd_renderer
{
public:
	virtual ~osd_renderer() {}
	virtual void display_message(const std::string &msg, uint64_t duration) = 0;
	virtual void add_rect(const osd_rect &rect) = 0;
	virtual void add_image(const osd_image &image) = 0;
	virtual void clear_canvas() = 0;
};
