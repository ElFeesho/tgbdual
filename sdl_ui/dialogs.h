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
#include "../goomba/goombarom.h"
#include "../goomba/goombasav.h"
#include "w32_posix.h"
#include "zlibwrap.h"

static int rom_size_tbl[] = {2, 4, 8, 16, 32, 64, 128, 256, 512};

typedef unsigned char BYTE;

char tmp_sram_name[256];
static const char mbc_types[0x101][40] = {
    "ROM Only", "ROM + MBC1", "ROM + MBC1 + RAM", "ROM + MBC1 + RAM + Battery", "Unknown", "ROM + MBC2", "ROM + MBC2 + Battery", "Unknown",
    "ROM + RAM", "ROM + RAM + Battery", "Unknown", "ROM + MMM01", "ROM + MMM01 + SRAM", "ROM + MMM01 + Battery", "Unknown",
    "ROM + MBC3 + TIMER + Battery", "ROM + MBC3 + TIMER + RAM + Battery", "ROM + MBC3", "ROM + MBC3 + RAM", "ROM + MBC3 + RAM + Battery",
    "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
    "ROM + MBC5", "ROM + MBC5 + RAM", "ROM + MBC5 + RAM + Battery", "ROM + MBC5 + RUMBLE", "ROM + MBC5 + RUMBLE + SRAM", "ROM + MBC5 + RUMBLE + SRAM + Battery",
    "Pocket Camera", "", "", "MBC7? + EEPROM + MOTIONSENSOR",                                           //#22
    "", "", "", "", "", "", "", "", "", "", "", "", "",                                                 //#2F
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#3F
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#4F
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#5F
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#6F
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#7F
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#8F
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#9F
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#AF
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#BF
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#CF
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#DF
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                     //#EF
    "", "", "", "", "", "", "", "", "", "", "", "", "", "Bandai TAMA5", "Hudson HuC-3", "Hudson HuC-1", //#FF
    "mmm01"                                                                                             // 逃げ
};

static byte org_gbtype;
static bool sys_win2000;
static int sram_tbl[] = {1, 1, 1, 4, 16, 8};
static bool goomba_load_error;

bool save_goomba(const void *buf, int size, int num, FILE *fs) {
    if (goomba_load_error) {
        // don't save data - error occured when first loading, and user was notified
        return true;
    } else {
        byte gba_data[GOOMBA_COLOR_SRAM_SIZE];
        fread(gba_data, 1, GOOMBA_COLOR_SRAM_SIZE, fs);
        fseek(fs, 0, SEEK_SET);

        void *cleaned = goomba_cleanup(gba_data);
        if (cleaned == NULL) {
            return false;
        } else if (cleaned != gba_data) {
            memcpy(gba_data, cleaned, GOOMBA_COLOR_SRAM_SIZE);
            free(cleaned);
        }

        stateheader *sh = stateheader_for(gba_data,
                                          g_gb->get_rom()->get_info()->cart_name);
        if (sh == NULL) {
            return false; // don't try to save sram
        }
        void *new_data = goomba_new_sav(gba_data, sh, buf, 0x2000 * sram_tbl[size]);
        if (new_data == NULL) {
            return false;
        }
        fwrite(new_data, 1, GOOMBA_COLOR_SRAM_SIZE, fs);
        return true;
    }
}

void save_sram(byte *buf, int size) {
    if (strstr(tmp_sram_name, ".srt"))
        return;

    char cur_di[256], sv_dir[256];
    GetCurrentDirectory(256, cur_di);
    config->get_save_dir(sv_dir);
    SetCurrentDirectory(sv_dir);

    FILE *fsu = fopen(tmp_sram_name, "r+b");
    if (fsu != NULL) {
        // if file exists, check for goomba
        uint32_t stateid = 0;
        fread(&stateid, 1, 4, fsu);
        fseek(fsu, 0, SEEK_SET);
        if (stateid == GOOMBA_STATEID) {
            if (!save_goomba(buf, size, 0, fsu)) {
                fprintf(stderr, "Could not save SRAM (Goomba format).\n(%s)\n", goomba_last_error());
            }
            fclose(fsu);
            SetCurrentDirectory(cur_di);
            return;
        } else {
            fclose(fsu);
        }
    }

    gzFile fs = gzopen(tmp_sram_name, "wb");
    gzwrite(fs, buf, 0x2000 * sram_tbl[size]);
    if ((g_gb->get_rom()->get_info()->cart_type >= 0x0f) && (g_gb->get_rom()->get_info()->cart_type <= 0x13)) {
        int tmp = render->get_timer_state();
        gzwrite(fs, &tmp, 4);
    }
    gzclose(fs);
    SetCurrentDirectory(cur_di);
}

void load_key_config(int num) {
    int buf[16];
    key_dat keys[8]; // a,b,select,start,down,up,left,right の順

    config->get_key_setting(buf, 0);

    keys[0].device_type = buf[0];
    keys[1].device_type = buf[2];
    keys[2].device_type = buf[4];
    keys[3].device_type = buf[6];
    keys[4].device_type = buf[8];
    keys[5].device_type = buf[10];
    keys[6].device_type = buf[12];
    keys[7].device_type = buf[14];
    keys[0].key_code = buf[1];
    keys[1].key_code = buf[3];
    keys[2].key_code = buf[5];
    keys[3].key_code = buf[7];
    keys[4].key_code = buf[9];
    keys[5].key_code = buf[11];
    keys[6].key_code = buf[13];
    keys[7].key_code = buf[15];

    render->set_key(keys);

    render->set_use_ffb(config->use_ffb);

    col_filter cof;
    cof.r_def = config->r_def;
    cof.g_def = config->g_def;
    cof.b_def = config->b_def;
    cof.r_div = config->r_div;
    cof.g_div = config->g_div;
    cof.b_div = config->b_div;
    cof.r_r = config->r_r;
    cof.r_g = config->r_g;
    cof.r_b = config->r_b;
    cof.g_r = config->g_r;
    cof.g_g = config->g_g;
    cof.g_b = config->g_b;
    cof.b_r = config->b_r;
    cof.b_g = config->b_g;
    cof.b_b = config->b_b;
    render->set_filter(&cof);
}

bool try_load_goomba(void *ram, int ram_size, gzFile fs, const char *cart_name, int num) {
    gzseek(fs, 0, SEEK_SET);
    char gba_data[GOOMBA_COLOR_SRAM_SIZE];
    gzread(fs, gba_data, GOOMBA_COLOR_SRAM_SIZE);
    gzclose(fs);

    void *cleaned = goomba_cleanup(gba_data);
    if (cleaned == NULL) {
        return false;
    } else if (cleaned != gba_data) {
        memcpy(gba_data, cleaned, GOOMBA_COLOR_SRAM_SIZE);
        free(cleaned);
    }

    stateheader *sh = stateheader_for(gba_data, cart_name);
    if (sh == NULL) {
        return false;
    }

    goomba_size_t output_size;
    void *gbc_data = goomba_extract(gba_data, sh, &output_size);
    if (gbc_data == NULL) {
        return false;
    }

    if (output_size > ram_size) {
        goomba_set_last_error("The extracted SRAM is too big for this ROM.");
        free(gbc_data);
        return false;
    } else {
        memcpy(ram, gbc_data, output_size);
        free(gbc_data);
        return true;
    }
}

int load_rom(char *romFile, bool isServer) {
    FILE *file;
    int size;
    BYTE *dat;
    char *p = romFile;

    p = strstr(romFile, ".");
    if (!p) {
        return -1;
    }

    while (*p != '\0') {
        *p = tolower(*p);
        p++;
    }

    static const char *exts[] = {"gb", "gbc", "sgb", "gba", 0};
    BYTE *tmpbuf = file_read(romFile, exts, &size);
    if (!tmpbuf) {
        return -1;
    }

    int num_roms = 0;
    for (const void *rom = gb_first_rom(tmpbuf, size); rom != NULL; rom = gb_next_rom(tmpbuf, size, rom)) {
        num_roms++;
    }

    if (num_roms == 0) {
        fprintf(stderr, "This file does not contain any Game Boy ROM images.");
        free(tmpbuf);
        return -1;
    } else if (num_roms == 1) {
        const void *first_rom = gb_first_rom(tmpbuf, size);
        size = gb_rom_size(first_rom);
        dat = (BYTE *)malloc(size);
        memcpy(dat, first_rom, size);
        free(tmpbuf);
    }

    if (!g_gb) {
        g_gb = new gb(render, true, true, isServer ? 1 : 0);

        if (config->sound_enable[4]) {
            g_gb->get_apu()->get_renderer()->set_enable(0, config->sound_enable[0] ? true : false);
            g_gb->get_apu()->get_renderer()->set_enable(1, config->sound_enable[1] ? true : false);
            g_gb->get_apu()->get_renderer()->set_enable(2, config->sound_enable[2] ? true : false);
            g_gb->get_apu()->get_renderer()->set_enable(3, config->sound_enable[3] ? true : false);
        } else {
            g_gb->get_apu()->get_renderer()->set_enable(0, false);
            g_gb->get_apu()->get_renderer()->set_enable(1, false);
            g_gb->get_apu()->get_renderer()->set_enable(2, false);
            g_gb->get_apu()->get_renderer()->set_enable(3, false);
        }
        g_gb->get_apu()->get_renderer()->set_echo(config->b_echo);
        g_gb->get_apu()->get_renderer()->set_lowpass(config->b_lowpass);
    } else {
        if (g_gb->has_battery()) {
            save_sram(g_gb->get_rom()->get_sram(), g_gb->get_rom()->get_info()->ram_size);
        }
    }

    char sram_name[256], cur_di[256], sv_dir[256];
    BYTE *ram;
    int ram_size = 0x2000 * sram_tbl[dat[0x149]];
    char *sram_name_only;
    strcpy(sram_name, romFile);
    strcpy(strstr(sram_name, "."), ".sav");
    sram_name_only = strrchr(sram_name, '/');
    if (!sram_name_only) {
        sram_name_only = sram_name;
    } else {
        sram_name_only++;
    }

    GetCurrentDirectory(256, cur_di);
    config->get_save_dir(sv_dir);
    SetCurrentDirectory(sv_dir);

    const char *cart_name = (const char *)dat + 0x134;
    gzFile fs = gzopen(sram_name_only, "rb");
    if (fs != nullptr) {
        ram = new BYTE[ram_size];
        memset(ram, 0, ram_size);
        gzread(fs, ram, ram_size);
        if (*(uint32_t *)ram == GOOMBA_STATEID) {
            goomba_load_error = !try_load_goomba(ram, ram_size, fs, cart_name, 0);
            if (goomba_load_error) {
                fprintf(stderr, "Goomba SRAM load error - your progress will not be saved.\n(%s)", goomba_last_error());
            }
        } else {
            gzseek(fs, 0, SEEK_END);
            if (gztell(fs) & 0xff) { // RTC won't work with gzip save files
                int tmp;
                gzseek(fs, -4, SEEK_END);
                gzread(fs, &tmp, 4);
                render->set_timer_state(tmp);
            }
            gzclose(fs);
        }
    } else {
        strcpy(strstr(sram_name_only, "."), ".ram");
        fs = gzopen(sram_name_only, "rb");
        ram = new BYTE[ram_size];
        memset(ram, 0, ram_size);
        if (fs != nullptr) {
            gzread(fs, ram, ram_size);
            if (*(uint32_t *)ram == GOOMBA_STATEID) {
                memset(ram, 0, ram_size); // in case try_load_goomba fails
                goomba_load_error = !try_load_goomba(ram, ram_size, fs, cart_name, 0);
                if (goomba_load_error) {
                    fprintf(stderr, "Goomba SRAM load error - your progress will not be saved.\n(%s)", goomba_last_error());
                }
            } else {
                gzseek(fs, 0, SEEK_END);
                if (gztell(fs) & 0xff) {
                    int tmp;
                    gzseek(fs, -4, SEEK_END);
                    gzread(fs, &tmp, 4);
                    render->set_timer_state(tmp);
                }
                gzclose(fs);
            }
        }
    }
    strcpy(strstr(sram_name_only, "."), ".sav");
    strcpy(tmp_sram_name, sram_name_only);

    SetCurrentDirectory(cur_di);

    org_gbtype = dat[0x143] & 0x80;

    if (config->gb_type == 1) {
        dat[0x143] &= 0x7f;
    } else if (config->gb_type >= 3) {
        dat[0x143] |= 0x80;
    }

    g_gb->set_use_gba(config->gb_type == 0 ? config->use_gba : (config->gb_type == 4 ? true : false));
    g_gb->load_rom(dat, size, ram, ram_size);

    free(dat);
    delete[] ram;

    SDL_WM_SetCaption(g_gb->get_rom()->get_info()->cart_name, 0);

    return 0;
}

void load_rom_only(char *buf) {
    FILE *file;
    int size;
    BYTE *dat;
    char *p = buf;

    p = strstr(buf, ".");
    for (int i = 0; i < strlen(p); i++) {
        p[i] = tolower(p[i]);
    }

    if (strstr(buf, ".gb") || strstr(buf, ".gbc")) {
        file = fopen(buf, "rb");
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);
        dat = (BYTE *)malloc(size);
        fread(dat, 1, size, file);
        fclose(file);
    }
    /// archive 対応必要
    else
        return;

    int tbl_ram[] = {1, 1, 1, 4, 16, 8}; //0と1は保険
    char sram_name[256], cur_di[256], sv_dir[256];
    BYTE *ram;
    int ram_size = 0x2000 * tbl_ram[dat[0x149]];
    char *_p = buf, *pp = buf;
    char *sram_name_only;
    while ((_p = strstr(pp, "\\")))
        pp = _p + 1;
    strcpy(sram_name, pp);
    strcpy(strstr(sram_name, "."), ".sav");
    sram_name_only = strrchr(sram_name, '/');
    if (!sram_name_only)
        sram_name_only = sram_name;
    else
        sram_name_only++;
    GetCurrentDirectory(256, cur_di);
    config->get_save_dir(sv_dir);
    SetCurrentDirectory(sv_dir);
    gzFile fs = gzopen(sram_name_only, "rb");
    if (fs) {
        ram = (BYTE *)malloc(ram_size);
        gzread(fs, ram, ram_size & 0xffffff00);
        gzseek(fs, 0, SEEK_END);
        if (gztell(fs) & 0xff) {
            int tmp;
            gzseek(fs, -4, SEEK_END);
            gzread(fs, &tmp, 4);
            render->set_timer_state(tmp);
        }
        gzclose(fs);
    } else {
        strcpy(strstr(sram_name_only, "."), ".ram");
        fs = gzopen(sram_name_only, "rb");
        if (fs != nullptr) {
            ram = (BYTE *)malloc(ram_size);
            gzread(fs, ram, ram_size & 0xffffff00);
            gzseek(fs, 0, SEEK_END);
            printf("%ld\n", gztell(fs));
            if (gztell(fs) & 0xff) {
                int tmp;
                gzseek(fs, -4, SEEK_END);
                gzread(fs, &tmp, 4);
                render->set_timer_state(tmp);
            }
            gzclose(fs);
        } else {
            ram = (BYTE *)malloc(ram_size);
            memset(ram, 0, ram_size);
        }
    }
    strcpy(strstr(sram_name_only, "."), ".sav");
    strcpy(tmp_sram_name, sram_name_only);

    SetCurrentDirectory(cur_di);

    org_gbtype = dat[0x143] & 0x80;

    if (config->gb_type == 1) {
        dat[0x143] &= 0x7f;
    } else if (config->gb_type >= 3) {
        dat[0x143] |= 0x80;
    }

    g_gb->set_use_gba(config->gb_type == 0 ? config->use_gba : (config->gb_type == 4 ? true : false));
    g_gb->load_rom(dat, size, ram, ram_size);
    free(dat);
    free(ram);

    SDL_WM_SetCaption(g_gb->get_rom()->get_info()->cart_name, 0);
}

static int elapse_wait = 0x10AAAA;

static void elapse_time(void) {
    static DWORD lastdraw = 0, rest = 0;
    DWORD t = timeGetTime();

    rest = (rest & 0xffff) + elapse_wait;

    DWORD wait = rest >> 16;
    DWORD elp = (DWORD)(t - lastdraw);

    if (elp >= wait) {
        lastdraw = t;
        return;
    }

    if (wait - elp >= 4)
        Sleep(wait - elp - 3);
    while ((timeGetTime() - lastdraw) < wait)
        ;

    lastdraw += wait;
}
