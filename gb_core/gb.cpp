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

//-------------------------------------------------
// GB その他エミュレーション部/外部とのインターフェース
// Interface with external / other unit emulation GB

#include "gb.h"
#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <stdexcept>
#include <string>


gb::gb(renderer *ref, bool b_lcd, bool b_apu, std::function<void()> sram_updated, std::function<uint8_t()> link_read, std::function<void(uint8_t)> link_write)
    : m_renderer{ref}, m_lcd{this}, m_cheat{this}, m_mbc{this}, m_apu{this}, m_cpu{this}, sram_update_cb{sram_updated}, link_read_cb{link_read}, link_write_cb{link_write} {

    m_renderer->reset();
    m_renderer->set_sound_renderer(b_apu ? m_apu.get_renderer() : nullptr);

    reset();

    use_gba = false;
}

void gb::reset() {
    regs.SC = 0;
    regs.DIV = 0;
    regs.TIMA = 0;
    regs.TMA = 0;
    regs.TAC = 0;
    regs.LCDC = 0x91;
    regs.STAT = 0;
    regs.SCY = 0;
    regs.SCX = 0;
    regs.LY = 153;
    regs.LYC = 0;
    regs.BGP = 0xFC;
    regs.OBP1 = 0xFF;
    regs.OBP2 = 0xFF;
    regs.WY = 0;
    regs.WX = 0;
    regs.IF = 0;
    regs.IE = 0;

    memset(&c_regs, 0, sizeof(c_regs));

    if (m_rom.get_loaded()) {
        m_rom.get_info()->gb_type = (m_rom.get_rom()[0x143] & 0x80) ? (use_gba ? 4 : 3) : 1;
    }
    m_cpu.reset();
    m_lcd.reset();
    m_apu.reset();
    m_mbc.reset();

    now_frame = 0;
    skip = skip_buf = 0;
    re_render = 0;
}

void gb::set_skip(int frame) {
    skip_buf = frame;
}

bool gb::load_rom(uint8_t *buf, int size, uint8_t *ram, int ram_size) {
    bool loadedSuccessfully = m_rom.load_rom(buf, size, ram, ram_size);
    if (loadedSuccessfully) {
        reset();
    }
    return loadedSuccessfully;
}

void gb::serialize(serializer &s) {
    s_VAR(regs);
    s_VAR(c_regs);

    m_rom.serialize(s);
    m_cpu.serialize(s);
    m_mbc.serialize(s);
    m_lcd.serialize(s);
    m_apu.serialize(s);
}

size_t gb::get_state_size(void) {
    size_t ret = 0;
    serializer s(&ret, serializer::COUNT);
    serialize(s);
    return ret;
}

void gb::save_state_mem(void *buf) {
    serializer s(buf, serializer::SAVE_BUF);
    serialize(s);
}

void gb::restore_state_mem(void *buf) {
    serializer s(buf, serializer::LOAD_BUF);
    serialize(s);
}

void gb::save_state(FILE *file) {
    serializer s(file, serializer::SAVE_FILE);
    serialize(s);
}

void gb::restore_state(FILE *file) {
    serializer s(file, serializer::LOAD_FILE);
    serialize(s);
}

void gb::refresh_pal() {
    for (int i = 0; i < 64; i++) {
        m_lcd.get_mapped_pal(i >> 2)[i & 3] = m_renderer->map_color(m_lcd.get_pal(i >> 2)[i & 3]);
    }
}

void gb::send_linkcable_byte(uint8_t data) {
    link_write_cb(data);
}

void gb::read_linkcable_byte(uint8_t *buff) {
    *buff = link_read_cb();
}

void gb::run() {
    for (int i = 0; i < 154; i++) {
        if (regs.LCDC & 0x80) {
            regs.LY = (regs.LY + 1) % 154;

            regs.STAT &= 0xF8;
            if (regs.LYC == regs.LY) {
                regs.STAT |= 4;
                if (regs.STAT & 0x40) {
                    m_cpu.irq(INT_LCDC);
                }
            }
            if (regs.LY == 0) {
                render_frame();
                skip = skip_buf;
            }
            if (regs.LY >= 144) {
                regs.STAT |= 1;
                if (regs.LY == 144) {
                    m_cpu.exec(72);
                    m_cpu.irq(INT_VBLANK);
                    if (regs.STAT & 0x10) {
                        m_cpu.irq(INT_LCDC);
                    }
                    m_cpu.exec(456 - 80);
                } else if (regs.LY == 153) {
                    m_cpu.exec(80);
                    regs.LY = 0;
                    m_cpu.exec(456 - 80);
                    regs.LY = 153;
                } else {
                    m_cpu.exec(456);
                }
            } else { // VBlank 期間外 // Period outside VBlank
                regs.STAT |= 2;
                if (regs.STAT & 0x20) {
                    m_cpu.irq(INT_LCDC);
                }
                m_cpu.exec(80); // state=2
                regs.STAT |= 3;
                m_cpu.exec(169); // state=3

                if (m_cpu.dma_executing) { // HBlank DMA
                    hblank_dma();
                } else {
                    regs.STAT &= 0xfc;
                    if (now_frame >= skip) {
                        m_lcd.render(vframe, regs.LY);
                    }
                    if ((regs.STAT & 0x08)) {
                        m_cpu.irq(INT_LCDC);
                    }
                    m_cpu.exec(207);
                }
            }
        } else {
            regs.LY = 0;
            re_render++;
            if (re_render >= 154) {
                memset(vframe, 0xff, 160 * 144 * 2);
                render_frame();
                re_render = 0;
            }
            regs.STAT &= 0xF8;
            m_cpu.exec(456);
        }
    }
}

bool gb::has_battery() {
    auto cart_type = m_rom.get_info()->cart_type;
    int has_bat[] = {0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    return has_bat[(cart_type > 0x20) ? 3 : cart_type];
}

void inline gb::render_frame() {
    m_renderer->refresh();
    if (now_frame >= skip) {
        m_renderer->render_screen((uint8_t *)vframe, 160, 144, 16);
        now_frame = 0;
    } else {
        now_frame++;
    }
    m_lcd.clear_win_count();
}

void inline gb::hblank_dma() {

    if (m_cpu.b_dma_first) {
        m_cpu.dma_dest_bank = m_cpu.vram_bank;
        if (m_cpu.dma_src < 0x4000) {
            m_cpu.dma_src_bank = m_rom.get_rom();
        } else if (m_cpu.dma_src < 0x8000) {
            m_cpu.dma_src_bank = m_mbc.get_rom();
        } else if (m_cpu.dma_src >= 0xA000 && m_cpu.dma_src < 0xC000) {
            printf("SRAM?\n");
            m_cpu.dma_src_bank = m_mbc.get_sram() - 0xA000;
        } else if (m_cpu.dma_src >= 0xC000 && m_cpu.dma_src < 0xD000) {
            m_cpu.dma_src_bank = m_cpu.ram - 0xC000;
        } else if (m_cpu.dma_src >= 0xD000 && m_cpu.dma_src < 0xE000) {
            m_cpu.dma_src_bank = m_cpu.ram_bank - 0xD000;
        } else {
            m_cpu.dma_src_bank = NULL;
        }
        m_cpu.b_dma_first = false;
    }

    memcpy(m_cpu.dma_dest_bank + (m_cpu.dma_dest & 0x1ff0), m_cpu.dma_src_bank + m_cpu.dma_src, 16);

    m_cpu.dma_src += 16;
    m_cpu.dma_src &= 0xfff0;
    m_cpu.dma_dest += 16;
    m_cpu.dma_dest &= 0xfff0;
    m_cpu.dma_rest--;

    if (!m_cpu.dma_rest) {
        m_cpu.dma_executing = false;
    }

    if (now_frame >= skip) {
        m_lcd.render(vframe, regs.LY);
    }

    regs.STAT &= 0xfc;
    m_cpu.exec(207);
}

void gb::notify_sram_written() {
    sram_update_cb();
}