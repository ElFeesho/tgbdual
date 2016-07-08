/*--------------------------------------------------
   TGB Dual - Gameboy Emulator -
   Copyright (C) 2001  Hii, 2014 libertyernie

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

#include <zlib.h>
#include "zlibwrap.h"

static int rom_size_tbl[] = {2, 4, 8, 16, 32, 64, 128, 256, 512};

typedef unsigned char BYTE;

static int sram_tbl[] = {1, 1, 1, 4, 16, 8};

uint8_t *file_read(const std::string &name, int *size) {
    uint8_t *dat = 0;
    FILE *file = fopen(name.c_str(), "rb");
    if (!file)
        return nullptr;
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    dat = (uint8_t *)malloc(*size);
    fread(dat, 1, *size, file);
    fclose(file);
    return dat;
}

void save_sram(gb *g_gb, int timer_state, const std::string &file_name, uint8_t *buf, int size) {
    FILE *fsu = fopen(file_name.c_str(), "w");
    fwrite((void*)buf, size, 1, fsu);
    fclose(fsu);
}

gb load_rom(const std::string &romFile, const std::string &save_file, sdl_renderer *render, std::function<void()> save_cb, std::function<uint8_t()> link_read_cb, std::function<void(uint8_t)> link_write_cb) {
    
    int size = 0;
    uint8_t *dat = file_read(romFile, &size);
    
    int ram_size;
    uint8_t *ram = file_read(save_file, &ram_size);
    
    gb g_gb{render, save_cb, link_read_cb, link_write_cb};

    dat[0x143] |= 0x80;
    
    g_gb.load_rom(dat, size, ram, ram_size);

    free(dat);
    free(ram);

    SDL_WM_SetCaption(g_gb.get_rom()->get_info()->cart_name, 0);

    return g_gb;
}

static int elapse_wait = 0x10AAAA;

static void elapse_time(void) {
    static uint32_t lastdraw = 0, rest = 0;
    uint32_t t = SDL_GetTicks();

    rest = (rest & 0xffff) + elapse_wait;

    uint32_t wait = rest >> 16;
    uint32_t elp = (uint32_t)(t - lastdraw);

    if (elp >= wait) {
        lastdraw = t;
        return;
    }

    if (wait - elp >= 4) {
        SDL_Delay(wait - elp - 3);
    }

    while ((SDL_GetTicks() - lastdraw) < wait)
        ;

    lastdraw += wait;
}
