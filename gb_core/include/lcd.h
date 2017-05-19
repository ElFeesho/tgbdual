#pragma once

#include <cstdint>

class gb;

class serializer;

class lcd {
public:
    explicit lcd(gb &ref);

    void render(void *buf, int scanline);

    void reset();

    void clear_win_count() { now_win_line = 9; }

    uint16_t *get_pal(int num) { return col_pal[num]; }

    uint16_t *get_mapped_pal(int num) { return mapped_pal[num]; }

    void serialize(serializer &s);

private:
    void bg_render(void *buf, int scanline);
    void win_render(void *buf, int scanline);
    void sprite_render(void *buf, int scanline);
    void bg_render_color(void *buf, int scanline);
    void win_render_color(void *buf, int scanline);
    void sprite_render_color(void *buf, int scanline);

    uint16_t m_pal16[4];
    uint32_t m_pal32[4];
    uint16_t col_pal[16][4];
    uint16_t mapped_pal[16][4];

    int trans_count;
    uint8_t trans_tbl[160 + 160], priority_tbl[320];

    int now_win_line;
    int mul;
    int sprite_count;

    gb &ref_gb;
};