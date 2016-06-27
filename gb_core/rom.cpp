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

//-----------------------------------------------
// ROMイメージ管理部 (含SRAM) // ROM image management unit (SRAM included)

#include "rom.h"
#include <memory.h>
#include <stdlib.h>
#include "serializer.h"

rom::rom() {
    b_loaded = false;

    dat = NULL;
    sram = NULL;
}

rom::~rom() {
    free(dat);
    free(sram);
}

bool rom::has_battery() {
    static const int32_t has_bat[] = {0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1,
                                      0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
                                      0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    return has_bat[(info.cart_type > 0x20) ? 3 : info.cart_type] == 1;
}

uint16_t rom::get_sram_size() {
    static const uint16_t tbl_ram[] = {1, 1, 1, 4, 16, 8};
    return 0x2000 * tbl_ram[info.ram_size];
}

bool rom::load_rom(uint8_t *buf, size_t size, uint8_t *ram, size_t ram_size) {
    uint8_t momocol_title[16] = {0x4D, 0x4F, 0x4D, 0x4F, 0x43, 0x4F, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    if (b_loaded) {
        free(dat);
        free(sram);
    }

    memcpy(info.cart_name, buf + 0x134, 16);
    info.cart_name[16] = '\0';
    info.cart_name[17] = '\0';
    info.cart_type = buf[0x147];
    info.rom_size = buf[0x148];
    info.ram_size = buf[0x149];

    if (memcmp(info.cart_name, momocol_title, 16) == 0) {
        info.cart_type = 0x100;
    }

    info.gb_type = (buf[0x143] & 0x80) ? 3 : 1;

    if (info.rom_size > 8) {
        return false;
    }

    dat = (uint8_t *)malloc(size);
    memcpy(dat, buf, size);
    first_page = dat;

    sram = (uint8_t *)malloc(get_sram_size());
    if (ram) {
        memcpy(sram, ram, ram_size & 0xffffff00);
    }

    b_loaded = true;

    return true;
}

void rom::serialize(serializer &s) {
    s_VAR(info);
    s.process(sram, get_sram_size());
}
