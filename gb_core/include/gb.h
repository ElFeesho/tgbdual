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

//--------------------------------------------------
// GB クラス定義部,その他

#pragma once

#include <cstdio>
#include <list>

#include "apu.h"
#include "cheat.h"
#include "cpu.h"
#include "lcd.h"
#include "mbc.h"
#include "video_renderer.h"
#include "rom.h"
#include "serializer.h"
#include "gbtime.h"

#include <functional>

#include "address_scanner.h"
#include "audio_renderer.h"

#define INT_VBLANK 1
#define INT_LCDC 2
#define INT_TIMER 4
#define INT_SERIAL 8
#define INT_PAD 16


struct gb_regs {
    uint8_t P1, SB, SC, DIV, TIMA, TMA, TAC, IF, LCDC, STAT, SCY, SCX, LY, LYC, DMA, BGP, OBP1, OBP2, WY, WX, IE;
};

struct gbc_regs {
    uint8_t KEY1, VBK, HDMA1, HDMA2, HDMA3, HDMA4, HDMA5, RP, BCPS, BCPD, OCPS, OCPD, SVBK;
};

class gamepad_source;

class gb {
    friend class cpu;

public:
    gb(video_renderer *ref, audio_renderer *audio, gamepad_source *gamepad_source_ref, std::function<void()> sram_updated, std::function<uint8_t()> link_read, std::function<void(uint8_t)> link_write);

    cpu &get_cpu();

    lcd &get_lcd();

    apu &get_apu();

    rom &get_rom();

    mbc &get_mbc();

    cheat &get_cheat();

    gb_regs &get_regs();

    gbc_regs &get_cregs();

    void run();

    void reset();

    void set_skip(int frame);

    void add_cheat(const std::string &cheat_code);

    bool load_rom(uint8_t *buf, size_t size, uint8_t *ram, size_t ram_size);

    void serialize(serializer &s);

    size_t get_state_size();

    void save_state_mem(void *buf);

    void restore_state_mem(void *buf);

    void send_linkcable_byte(uint8_t data);

    void read_linkcable_byte(uint8_t *buff);

    void notify_sram_written();

    uint16_t map_color(uint16_t gb_col);

    uint16_t get_sensor(bool x_y);

    uint8_t get_time(int type);

    void set_time(int type, uint8_t dat);

    uint8_t check_pad();

    int32_t gb_type();

    void inline render_frame();

    void inline hblank_dma();

    address_scanner create_address_scanner();

private:
    video_renderer *m_renderer;
    gamepad_source *m_gamepad;

    lcd m_lcd;
    cpu m_cpu;
    apu m_apu;
    rom m_rom;
    mbc m_mbc;
    gbtime _time;

    cheat m_cheat;

    gb_regs regs;
    gbc_regs c_regs;

    uint16_t vframe[160 * (144 + 100)];
    uint32_t vframe_32bit[160 * (144 + 100)];

    int skip;
    int skip_buf;
    int now_frame;
    int re_render;

    std::function<void()> sram_update_cb;
    std::function<uint8_t()> link_read_cb;
    std::function<void(uint8_t)> link_write_cb;
};
