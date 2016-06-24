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

#include <list>
#include "../gb_core/gb.h"

#include "dmy_renderer.h"
#include "resource.h"
#include "sdl_renderer.h"
#include "setting.h"
#include "sock.h"
#include "w32_posix.h"

#include <SDL.h>

#define hide

static bool sram_transfer_rest = false;
bool b_running = true;

gb *g_gb[2];
///gbr *g_gbr;
sdl_renderer *render[2];
#ifndef hide
dx_renderer *dmy_render;
#else
dmy_renderer *dmy_render;
#endif
setting *config;
sock *g_sock;
std::list<char *> mes_list, chat_list;
bool chat_rest = false;
char chat_send[256];
Uint8 *key_state;

bool b_terminal = false;

FILE *key_file;
FILE *mov_file;

bool endGame;

#include "dialogs.h"

int main(int argc, char *argv[]) {
    char cur_dir[256];
    GetCurrentDirectory(256, cur_dir);

    printf("Current directory: %s\n", cur_dir);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    config = new setting();

    render[0] = new sdl_renderer();
    render[1] = nullptr;
    dmy_render = nullptr;

    g_gb[0] = g_gb[1] = nullptr;

    render[0]->set_render_pass(config->render_pass);
    render[0]->show_fps(config->show_fps);

    printf("load key config\n");

    load_key_config(0);

    key_dat tmp_save_load;
    tmp_save_load.device_type = config->save_key[0];
    tmp_save_load.key_code = config->save_key[1];
    render[0]->set_save_key(&tmp_save_load);
    tmp_save_load.device_type = config->load_key[0];
    tmp_save_load.key_code = config->load_key[1];
    render[0]->set_load_key(&tmp_save_load);
    tmp_save_load.device_type = config->auto_key[0];
    tmp_save_load.key_code = config->auto_key[1];
    render[0]->set_auto_key(&tmp_save_load);
    tmp_save_load.device_type = config->pause_key[0];
    tmp_save_load.key_code = config->pause_key[1];
    render[0]->set_pause_key(&tmp_save_load);
    tmp_save_load.device_type = config->full_key[0];
    tmp_save_load.key_code = config->full_key[1];
    render[0]->set_full_key(&tmp_save_load);
    tmp_save_load.device_type = config->reset_key[0];
    tmp_save_load.key_code = config->reset_key[1];
    render[0]->set_reset_key(&tmp_save_load);
    tmp_save_load.device_type = config->quit_key[0];
    tmp_save_load.key_code = config->quit_key[1];
    render[0]->set_quit_key(&tmp_save_load);

    endGame = false;

    purse_cmdline(argc, argv);
    if (argc >= 2) {
        SetCurrentDirectory(cur_dir);
        printf("load rom %s\n", argv[1]);
        if (load_rom(argv[1], 0, argc == 3) != 0) {
            printf("ERROR: invalid rom, usage: %s rom_name\n", argv[0]);
            endGame = true;
        }
    } else {
        printf("ERROR: usage: %s rom_name\n", argv[0]);
        endGame = true;
    }

    int line = 0;
    int phase = 0;

    printf("start.\n");

    while (!endGame) {
        // 一時的に
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    endGame = true;
                    break;
            }
        }
        if (key_state[SDLK_ESCAPE])
            endGame = true;

        if (!b_running) {
            if (render[0])
                render[0]->refresh();
            continue;
        }

        render[0]->enable_check_pad();

        for (line = 0; line < 154; line++) {
            if (g_gb[0])
                g_gb[0]->run();
            if (g_gb[1])
                g_gb[1]->run();
        }


        key_dat tmp_key;
        tmp_key.device_type = config->fast_forwerd[0];
        tmp_key.key_code = config->fast_forwerd[1];
        if (!render[0]->check_press(&tmp_key)) { // 通常
            if (g_gb[0])
                g_gb[0]->set_skip(config->frame_skip);
            if (render[0])
                render[0]->set_mul(config->frame_skip + 1);
            if (config->speed_limit) {
                elapse_wait = (1000 << 16) / config->virtual_fps;
                elapse_time();
            }
        } else { // Fast Forwerd 状態
            if (g_gb[0])
                g_gb[0]->set_skip(config->fast_frame_skip);
            if (render[0])
                render[0]->set_mul(config->fast_frame_skip + 1);
            if (config->fast_speed_limit) {
                elapse_wait = (1000 << 16) / config->fast_virtual_fps;
                elapse_time();
            }
        }
    }

    // from WM_DESTROY
    int has_bat[] = {0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 0x20以下
    if (g_gb[0]) {
        if (has_bat[(g_gb[0]->get_rom()->get_info()->cart_type > 0x20) ? 3 : g_gb[0]->get_rom()->get_info()->cart_type])
            save_sram(g_gb[0]->get_rom()->get_sram(), g_gb[0]->get_rom()->get_info()->ram_size, 0);
        delete g_gb[0];
        g_gb[0] = nullptr;
    }
    if (g_gb[1]) {
        if (has_bat[(g_gb[1]->get_rom()->get_info()->cart_type > 0x20) ? 3 : g_gb[1]->get_rom()->get_info()->cart_type])
            save_sram(g_gb[1]->get_rom()->get_sram(), g_gb[1]->get_rom()->get_info()->ram_size, 1);
        delete g_gb[1];
        g_gb[1] = nullptr;
    }

    if (render[0]) {
        delete render[0];
        render[0] = nullptr;
    }
    if (render[1]) {
        delete render[1];
        render[1] = nullptr;
    }
    mes_list.clear();
    chat_list.clear();

    delete config;

    return 0;
}
