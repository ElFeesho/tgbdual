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

#include <stdio.h>
#include <list>

#include "apu.h"
#include "cheat.h"
#include "cpu.h"
#include "lcd.h"
#include "mbc.h"
#include "renderer.h"
#include "rom.h"
#include "serializer.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <functional>

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


class gb {
    friend class cpu;

   public:
    gb(renderer *ref, bool b_lcd, bool b_apu, std::function<void()> sram_updated, std::function<uint8_t()> link_read, std::function<void(uint8_t)> link_write);
    
    cpu *get_cpu() { return &m_cpu; }
    lcd *get_lcd() { return &m_lcd; }
    apu *get_apu() { return &m_apu; }
    rom *get_rom() { return &m_rom; }
    mbc *get_mbc() { return &m_mbc; }
    renderer *get_renderer() { return m_renderer; }
    cheat *get_cheat() { return &m_cheat; }

    gb_regs *get_regs() { return &regs; }
    gbc_regs *get_cregs() { return &c_regs; }

    void run();
    void reset();
    void set_skip(int frame);
    void set_use_gba(bool use) { use_gba = use; }
    bool load_rom(uint8_t *buf, int size, uint8_t *ram, int ram_size);

    void serialize(serializer &s);

    size_t get_state_size(void);
    void save_state_mem(void *buf);
    void restore_state_mem(void *buf);
    void save_state(FILE *file);
    void restore_state(FILE *file);

    void refresh_pal();

    void send_linkcable_byte(uint8_t data);
    void read_linkcable_byte(uint8_t *buff);

    bool has_battery();

    void notify_sram_written();

    void inline render_frame();
    void inline hblank_dma();

   private:
    renderer *m_renderer;
    lcd m_lcd;
    cpu m_cpu;
    apu m_apu;
    rom m_rom;
    mbc m_mbc;

    cheat m_cheat;

    gb_regs regs;
    gbc_regs c_regs;

    uint16_t dmy[160 * 5]; 
    uint16_t vframe[160 * (144 + 100)];

    int skip, skip_buf;
    int now_frame;
    int re_render;

    bool use_gba;

    int nt_mode;

    int net_socket;
    struct sockaddr_in theiraddr, myaddr;

    std::function<void()> sram_update_cb;
    std::function<uint8_t()> link_read_cb;
    std::function<void(uint8_t)> link_write_cb;
};
