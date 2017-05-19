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

#include "gamepad_source.h"

static uint32_t convert_to_second(struct tm *sys);

gb::gb(video_renderer *ref, audio_renderer *audio, gamepad_source *gamepad_source_ref, std::function<void()> sram_updated, std::function<uint8_t()> link_read, std::function<void(uint8_t)> link_write)
        : m_renderer{ref},
          m_gamepad{gamepad_source_ref},
          m_lcd{*this},
          m_cpu{*this},
          m_apu{*this},
          m_mbc{*this},
          m_cheat{},
          sram_update_cb{std::move(sram_updated)},
          link_read_cb{std::move(link_read)},
          link_write_cb{std::move(link_write)} {

    audio->connect_audio_provider(m_apu.get_stream_provider());

    reset();
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

void gb::add_cheat(const std::string &cheat_code) {
    m_cheat.add_cheat(cheat_code, std::bind(&cpu::write, &m_cpu, std::placeholders::_1, std::placeholders::_2));
}

bool gb::load_rom(uint8_t *buf, size_t size, uint8_t *ram, size_t ram_size) {
    reset();
    return m_rom.load_rom(buf, size, ram, ram_size);
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

size_t gb::get_state_size() {
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

void gb::send_linkcable_byte(uint8_t data) {
    link_write_cb(data);
}

void gb::read_linkcable_byte(uint8_t *buff) {
    *buff = link_read_cb();
}

void gb::run() {
    for (int i = 0; i < 154; i++) {
        if ((regs.LCDC & 0x80) != 0) {
            regs.LY = (uint8_t) ((regs.LY + 1) % 154);

            regs.STAT &= 0xF8;
            if (regs.LYC == regs.LY) {
                regs.STAT |= 4;
                if ((regs.STAT & 0x40) != 0) {
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
                    if ((regs.STAT & 0x10) != 0) {
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
                if ((regs.STAT & 0x20) != 0) {
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
                    if ((regs.STAT & 0x08) != 0) {
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
                memset(vframe_32bit, 0xff, 160 * 144 * 4);
                render_frame();
                re_render = 0;
            }
            regs.STAT &= 0xF8;
            m_cpu.exec(456);
        }
    }
}

static uint32_t rgb565_to8888(uint16_t c) {
    static float rbFactory = 255.f / 31.f;
    static float gFactory = 255.f / 63.f;
    uint32_t r = (uint32_t) (((c >> 11) & 0x1f) * rbFactory) << 0;
    uint32_t g = (uint32_t) (((c >> 5) & 0x3f) * gFactory) << 8;
    uint32_t b = (uint32_t) (((c & 0x1f)) * rbFactory) << 16;
    return (uint32_t) (r | g | b) | 0xff000000;
}

void inline gb::render_frame() {

    if (now_frame >= skip) {
        for (unsigned int i = 0; i < 160 * 144; i++) {
            vframe_32bit[i] = rgb565_to8888(vframe[i]);
        }

        m_renderer->render_screen((uint8_t *) vframe_32bit, 160, 144);
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
            m_cpu.dma_src_bank = m_mbc.get_sram() - 0xA000;
        } else if (m_cpu.dma_src >= 0xC000 && m_cpu.dma_src < 0xD000) {
            m_cpu.dma_src_bank = m_cpu.ram - 0xC000;
        } else if (m_cpu.dma_src >= 0xD000 && m_cpu.dma_src < 0xE000) {
            m_cpu.dma_src_bank = m_cpu.ram_bank - 0xD000;
        } else {
            m_cpu.dma_src_bank = nullptr;
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

uint16_t gb::get_sensor(bool /*unused*/) {
    return 0;
}

uint8_t gb::get_time(int type) {
    return _time.get_time(type);
}

void gb::set_time(int type, uint8_t dat) {
    _time.set_time(type, dat);
}

uint16_t gb::map_color(uint16_t gb_col) {
    return (uint16_t) (((gb_col & 0x1F) << 11) | ((gb_col & 0x3e0) << 1) | ((gb_col & 0x7c00) >> 10) | ((gb_col & 0x8000) >> 10));
}

void gb::notify_sram_written() {
    sram_update_cb();
}

uint8_t gb::check_pad() {
    return m_gamepad->check_pad();
}

address_scanner gb::create_address_scanner() {
    return {m_cpu.ram, 0x2000 * 4};
}

gbc_regs &gb::get_cregs() { return c_regs; }

gb_regs &gb::get_regs() { return regs; }

cheat &gb::get_cheat() { return m_cheat; }

mbc &gb::get_mbc() { return m_mbc; }

rom &gb::get_rom() { return m_rom; }

apu &gb::get_apu() { return m_apu; }

cpu &gb::get_cpu() { return m_cpu; }

lcd &gb::get_lcd() { return m_lcd; }

int32_t gb::gb_type() { return m_rom.get_info()->gb_type; }