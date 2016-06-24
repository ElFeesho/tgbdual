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

#include "resource.h"
#include "sdl_renderer.h"
#include "setting.h"
#include "w32_posix.h"

#include <SDL.h>

static bool sram_transfer_rest = false;

gb *g_gb;

sdl_renderer *render;

setting *config;

bool chat_rest = false;
char chat_send[256];
Uint8 *key_state;

FILE *key_file;
FILE *mov_file;

bool endGame;

#include "dialogs.h"

int main(int argc, char *argv[]) {
    char cur_dir[256];
    GetCurrentDirectory(256, cur_dir);


    config = new setting();
    render = new sdl_renderer();

    g_gb = nullptr;

    render->set_render_pass(config->render_pass);
    render->show_fps(config->show_fps);

    printf("load key config\n");

    load_key_config(0);

    key_dat tmp_save_load;
    tmp_save_load.device_type = config->save_key[0];
    tmp_save_load.key_code = config->save_key[1];
    render->set_save_key(&tmp_save_load);
    tmp_save_load.device_type = config->load_key[0];
    tmp_save_load.key_code = config->load_key[1];
    render->set_load_key(&tmp_save_load);
    tmp_save_load.device_type = config->auto_key[0];
    tmp_save_load.key_code = config->auto_key[1];
    render->set_auto_key(&tmp_save_load);
    tmp_save_load.device_type = config->pause_key[0];
    tmp_save_load.key_code = config->pause_key[1];
    render->set_pause_key(&tmp_save_load);
    tmp_save_load.device_type = config->full_key[0];
    tmp_save_load.key_code = config->full_key[1];
    render->set_full_key(&tmp_save_load);
    tmp_save_load.device_type = config->reset_key[0];
    tmp_save_load.key_code = config->reset_key[1];
    render->set_reset_key(&tmp_save_load);
    tmp_save_load.device_type = config->quit_key[0];
    tmp_save_load.key_code = config->quit_key[1];
    render->set_quit_key(&tmp_save_load);

    endGame = false;

    purse_cmdline(argc, argv);
    if (argc >= 2) {
        SetCurrentDirectory(cur_dir);
        printf("load rom %s\n", argv[1]);
        if (load_rom(argv[1], argc == 3) != 0) {
            printf("ERROR: invalid rom, usage: %s rom_name\n", argv[0]);
            endGame = true;
        }
    } else {
        printf("ERROR: usage: %s rom_name\n", argv[0]);
        endGame = true;
    }

    int line = 0;
    int phase = 0;

    while (!endGame) {

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                endGame = true;
                break;
            }
        }

        if (key_state[SDLK_ESCAPE]) {
            endGame = true;
        }

        render->enable_check_pad();

        for (line = 0; line < 154; line++) {
            g_gb->run();
        }

        key_dat tmp_key;
        tmp_key.device_type = config->fast_forwerd[0];
        tmp_key.key_code = config->fast_forwerd[1];
        if (!render->check_press(&tmp_key)) {
            g_gb->set_skip(config->frame_skip);
            render->set_mul(config->frame_skip + 1);

            if (config->speed_limit) {
                elapse_wait = (1000 << 16) / config->virtual_fps;
                elapse_time();
            }
        } else { // Fast Forwerd 状態
            g_gb->set_skip(config->fast_frame_skip);
            render->set_mul(config->fast_frame_skip + 1);
            if (config->fast_speed_limit) {
                elapse_wait = (1000 << 16) / config->fast_virtual_fps;
                elapse_time();
            }
        }
    }

    // from WM_DESTROY
    int has_bat[] = {0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (has_bat[(g_gb->get_rom()->get_info()->cart_type > 0x20) ? 3 : g_gb->get_rom()->get_info()->cart_type]) {
        save_sram(g_gb->get_rom()->get_sram(), g_gb->get_rom()->get_info()->ram_size, 0);
    }

    delete g_gb;
    g_gb = nullptr;

    delete render;
    render = nullptr;

    delete config;

    return 0;
}
