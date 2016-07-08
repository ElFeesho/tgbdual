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

void save_sram(gb *g_gb, const std::string &file_name, uint8_t *buf, int size) {
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
    g_gb.load_rom(dat, size, ram, ram_size);

    free(dat);
    free(ram);

    SDL_WM_SetCaption(g_gb.get_rom()->get_info()->cart_name, 0);

    return g_gb;
}

