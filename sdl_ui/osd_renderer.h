//
// Created by Christopher Sawczuk on 15/07/2016.
//

#pragma once

#include <cstdint>
#include <string>

class osd_rect
{
public:
	osd_rect(int x, int y, int w, int h) : _x{x}, _y{y}, _w{w}, _h{h} {}

	int x() { return _x; }
	int y() { return _y; }
	int w() { return _w; }
	int h() { return _h; }

private:
	int _x;
	int _y;
	int _w;
	int _h;
};

class osd_renderer
{
public:
	virtual ~osd_renderer() {}
	virtual void display_message(const std::string &msg, uint64_t duration) = 0;
	virtual void add_rect(const osd_rect &rect, int stroke_colour, int fill_colour);
};
