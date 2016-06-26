#include "callbacks.h"

#include "sdl_renderer.h"
#include "setting.h"
#include "w32_posix.h"

#include "../gb_core/gb.h"

#include <SDL.h>

#include <functional>

extern gb *g_gb;
extern setting *config;
extern sdl_renderer *render;
extern char tmp_sram_name[256];

void in_directory(char *dir, std::function<void()> dothis) {
    char cur_di[256];
    GetCurrentDirectory(256, cur_di);

    SetCurrentDirectory(dir);

    dothis();

    SetCurrentDirectory(cur_di);
}

void cb_save_state(int lParam) {
    char sv_dir[256];
    config->get_save_dir(sv_dir);

    in_directory(sv_dir, [&]() {
        char name[256], *p;
        strcpy(name, tmp_sram_name);
        if (!(p = strstr(name, ".sav"))) {
            if (!(p = strstr(name, ".ram"))) {
                return;
            }
        }

        sprintf(p, ".sv0");

        FILE *file = fopen(name, "wb");
        g_gb->save_state(file);
        int tmp = render->get_timer_state();
        fseek(file, -100, SEEK_CUR);
        fwrite(&tmp, 4, 1, file);
        fclose(file);
    });
}

void cb_load_state(int lParam) {
    char sv_dir[256];
    config->get_save_dir(sv_dir);

    in_directory(sv_dir, [&]() {
        char name[256], *p;
        strcpy(name, tmp_sram_name);
        if (!(p = strstr(name, ".sav"))) {
            if (!(p = strstr(name, ".ram"))) {
                return;
            }
        }

        sprintf(p, ".sv0");

        FILE *file = fopen(name, "rb");

        if (file) {
            g_gb->restore_state(file);
            int tmp;
            fseek(file, -100, SEEK_CUR);
            fread(&tmp, 4, 1, file);
            render->set_timer_state(tmp);
            fclose(file);
        }
    });
}
