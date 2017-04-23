#pragma once

#include <cstdint>
#include <string>

class osd_rect {
public:
    osd_rect() = default;;

    osd_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t strokeColour, uint32_t fillColour) : _x(x), _y{y}, _w{static_cast<uint16_t>(w)}, _h{static_cast<uint16_t>(h)},
                                                                                                       _strokeColour{strokeColour}, _fillColour{fillColour} {}

    int16_t x() const { return _x; }

    int16_t y() const { return _y; }

    uint16_t w() const { return _w; }

    uint16_t h() const { return _h; }

    uint32_t stroke() const { return _strokeColour; }

    uint32_t fill() const { return _fillColour; }

private:
    int16_t _x;
    int16_t _y;
    uint16_t _w;
    uint16_t _h;
    uint32_t _strokeColour;
    uint32_t _fillColour;
};

class osd_image {
public:
    osd_image(const std::string &name, int16_t x, int16_t y) : _name{name}, _x{x}, _y{y} {}

    std::string name() const { return _name; }

    int16_t x() const { return _x; }

    int16_t y() const { return _y; }

private:
    std::string _name;
    int16_t _x;
    int16_t _y;
};

class osd_renderer {
public:
    virtual ~osd_renderer() = default;

    virtual void display_message(const std::string &msg, uint64_t duration) = 0;

    virtual void add_rect(const osd_rect &rect) = 0;

    virtual void add_image(const osd_image &image) = 0;

    virtual void add_text(const std::string &text, int16_t x, int16_t y) = 0;
};
