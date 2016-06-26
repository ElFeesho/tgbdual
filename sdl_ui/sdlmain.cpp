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

gb *g_gb;
sdl_renderer *render;
setting *config;
bool endGame;

#include "dialogs.h"

int main(int argc, char *argv[]) {
    char cur_dir[256];
    GetCurrentDirectory(256, cur_dir);

    config = new setting();
    render = new sdl_renderer();

    render->set_render_pass(config->render_pass);

    load_key_config(0);

    key_dat tmp_save = {config->save_key[0], config->save_key[1]};
    render->set_save_key(&tmp_save);

    key_dat tmp_load = {config->load_key[0], config->load_key[1]};
    render->set_load_key(&tmp_load);

    key_dat tmp_auto = {config->auto_key[0], config->auto_key[1]};
    render->set_auto_key(&tmp_auto);

    key_dat fast_forward = {config->fast_forwerd[0], config->fast_forwerd[1]};

    endGame = false;

    if (argc >= 2) {
        SetCurrentDirectory(cur_dir);
        if (load_rom(argv[1], argc == 3) != 0) {
            printf("ERROR: invalid rom, usage: %s rom_name\n", argv[0]);
            endGame = true;
        }
    } else {
        printf("ERROR: usage: %s rom_name\n", argv[0]);
        endGame = true;
    }

    SDL_Event e;

    while (!endGame) {

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                endGame = true;
                break;
            }
        }

        g_gb->run();

        if (!render->check_press(&fast_forward)) {
            g_gb->set_skip(config->frame_skip);

            if (config->speed_limit) {
                elapse_wait = (1000 << 16) / config->virtual_fps;
                elapse_time();
            }
        } else {
            g_gb->set_skip(config->fast_frame_skip);
            if (config->fast_speed_limit) {
                elapse_wait = (1000 << 16) / config->fast_virtual_fps;
                elapse_time();
            }
        }
    }

    if (g_gb->has_battery()) {
        save_sram(g_gb->get_rom()->get_sram(), g_gb->get_rom()->get_info()->ram_size);
    }

    delete g_gb;
    delete render;
    delete config;

    return 0;
}
