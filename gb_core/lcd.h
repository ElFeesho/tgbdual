#pragma once

class gb;


class lcd {
   public:
    lcd(gb *ref);

    void render(void *buf, int scanline);
    void reset();
    void clear_win_count() { now_win_line = 9; }
    word *get_pal(int num) { return col_pal[num]; }
    word *get_mapped_pal(int num) { return mapped_pal[num]; }

    void set_enable(int layer, bool enable);
    bool get_enable(int layer);

    int get_sprite_count() { return sprite_count; };

    void serialize(serializer &s);

   private:
    void bg_render(void *buf, int scanline);
    void win_render(void *buf, int scanline);
    void sprite_render(void *buf, int scanline);
    void bg_render_color(void *buf, int scanline);
    void win_render_color(void *buf, int scanline);
    void sprite_render_color(void *buf, int scanline);

    word m_pal16[4];
    dword m_pal32[4];
    word col_pal[16][4];
    word mapped_pal[16][4];

    int trans_count;
    uint8_t trans_tbl[160 + 160], priority_tbl[320];

    int now_win_line;
    int mul;
    int sprite_count;

    bool layer_enable[3];

    gb *ref_gb;
};