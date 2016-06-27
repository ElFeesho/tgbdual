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

#include "gb_types.h"
#include "renderer.h"
#include "rom.h"
#include "lcd.h"
#include "cheat.h"
#include "apu.h"
#include "mbc.h"
#include "serializer.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define INT_VBLANK 1
#define INT_LCDC 2
#define INT_TIMER 4
#define INT_SERIAL 8
#define INT_PAD 16

class gb;
class cpu;
class apu;
class apu_snd;
class rom;

struct gb_regs {
    byte P1, SB, SC, DIV, TIMA, TMA, TAC, IF, LCDC, STAT, SCY, SCX, LY, LYC, DMA,
        BGP, OBP1, OBP2, WY, WX, IE;
};

struct gbc_regs {
    byte KEY1, VBK, HDMA1, HDMA2, HDMA3, HDMA4, HDMA5, RP, BCPS, BCPD, OCPS, OCPD,
        SVBK;
};

union pare_reg {
    word w;
    struct {
        byte l, h;
    } b;
};

struct cpu_regs {
    pare_reg AF;
    pare_reg BC;
    pare_reg DE;
    pare_reg HL;
    word SP;
    word PC;
    byte I;
};



class gb {
    friend class cpu;

   public:
    gb(renderer *ref, bool b_lcd, bool b_apu, int network_mode = 0);
    ~gb();

    cpu *get_cpu() { return m_cpu; }
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
    bool load_rom(byte *buf, int size, byte *ram, int ram_size);

    void serialize(serializer &s);

    size_t get_state_size(void);
    void save_state_mem(void *buf);
    void restore_state_mem(void *buf);
    void save_state(FILE *file);
    void restore_state(FILE *file);

    void refresh_pal();

    void send_linkcable_byte(byte data);
    void read_linkcable_byte(byte *buff);

    bool has_battery();

    void inline render_frame();
    void inline hblank_dma();

   private:
    renderer *m_renderer;
    lcd m_lcd;
    cpu *m_cpu;
    apu m_apu;
    rom m_rom;
    mbc m_mbc;

    cheat m_cheat;

    gb_regs regs;
    gbc_regs c_regs;

    word dmy[160 * 5]; // vframe はみ出した時用
    word vframe[160 * (144 + 100)];

    int skip, skip_buf;
    int now_frame;
    int re_render;

    bool use_gba;

    int nt_mode;

    int net_socket;
    struct sockaddr_in theiraddr, myaddr;
};

class cpu {
    friend class gb;

   public:
    cpu(gb *ref);
    ~cpu();

    byte read(word adr) {
        return (ref_gb->get_cheat()->get_cheat_map()[adr])
                   ? ref_gb->get_cheat()->cheat_read(adr)
                   : read_direct(adr);
    }

    byte read_direct(word adr);
    void write(word adr, byte dat);
    word inline readw(word adr) { return read(adr) | (read(adr + 1) << 8); }
    void inline writew(word adr, word dat) {
        write(adr, (byte)dat);
        write(adr + 1, dat >> 8);
    }

    void exec(int clocks);
    byte seri_send(byte dat);
    void irq(int irq_type);
    void inline irq_process();
    void reset();
    void set_trace(bool trace) { b_trace = trace; }

    byte *get_vram() { return vram; }
    byte *get_ram() { return ram; }
    byte *get_oam() { return oam; }
    byte *get_stack() { return stack; }

    byte *get_ram_bank() { return ram_bank; }
    void set_ram_bank(int bank) { ram_bank = ram + bank * 0x1000; }

    cpu_regs *get_regs() { return &regs; }

    int get_clock() { return total_clock; }
    bool get_speed() { return speed; }

    bool *get_halt() { return &halt; }

    void save_state(int *dat);
    void restore_state(int *dat);
    void save_state_ex(int *dat);
    void restore_state_ex(int *dat);

    void serialize(serializer &s);

   private:
    byte inline io_read(word adr);
    void inline io_write(word adr, byte dat);
    byte op_read() { return read(regs.PC++); }
    word op_readw() {
        regs.PC += 2;
        return readw(regs.PC - 2);
    }

    int dasm(char *S, byte *A);
    void log();

    gb *ref_gb;
    cpu_regs regs;

    byte ram[0x2000 * 4];
    byte vram[0x2000 * 2];
    byte stack[0x80];
    byte oam[0xA0];
    byte spare_oam[0x18];
    byte ext_mem[16];

    byte *vram_bank;
    byte *ram_bank;

    byte z802gb[256], gb2z80[256];
    dword rp_que[256];
    int que_cur;
    //	word org_pal[16][4];
    int total_clock, rest_clock, sys_clock, seri_occer, div_clock;
    bool halt, speed, speed_change, dma_executing;
    bool b_trace;
    int dma_src;
    int dma_dest;
    int dma_rest;
    int gdma_rest;
    bool b_dma_first;

    int last_int;
    bool int_desable;

    byte *dma_src_bank;
    byte *dma_dest_bank;

    byte _ff6c, _ff72, _ff73, _ff74, _ff75;

    //	FILE *file;
};
