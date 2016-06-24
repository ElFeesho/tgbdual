#include "callbacks.h"

#include "sdl_renderer.h"
#include "setting.h"
#include "w32_posix.h"

#include "../gb_core/gb.h"

#include <SDL.h>

extern gb *g_gb;
extern setting *config;
extern Uint8 *key_state;
extern sdl_renderer *render;
extern char tmp_sram_name[256];

void cb_save_state(int lParam) {
    char cur_di[256], sv_dir[256];
    GetCurrentDirectory(256, cur_di);
    config->get_save_dir(sv_dir);
    SetCurrentDirectory(sv_dir);

    char name[256], *p;
    strcpy(name, tmp_sram_name);
    if (!(p = strstr(name, ".sav")))
        if (!(p = strstr(name, ".ram")))
            return;
    int sav_slot;

    if ((int)lParam == -1) {
        sav_slot = ((key_state['1']) ? 1 : ((key_state['2']) ? 2 : ((key_state['3']) ? 3 : ((key_state['4']) ? 4 : ((key_state['5']) ? 5 : ((key_state['6']) ? 6 : ((key_state['7']) ? 7 : ((key_state['8']) ? 8 : ((key_state['9']) ? 9 : 0)))))))));
    } else {
        sav_slot = (int)lParam;
    }

    sprintf(p, ".sv%d", sav_slot);

    FILE *file = fopen(name, "wb");
    g_gb->save_state(file);
    int tmp = render->get_timer_state();
    fseek(file, -100, SEEK_CUR);
    fwrite(&tmp, 4, 1, file);
    fclose(file);

    SetCurrentDirectory(cur_di);
}

void cb_load_state(int lParam) {
    char cur_di[256], sv_dir[256];

    GetCurrentDirectory(256, cur_di);
    config->get_save_dir(sv_dir);
    SetCurrentDirectory(sv_dir);

    char name[256], *p;
    strcpy(name, tmp_sram_name);
    if (!(p = strstr(name, ".sav")))
        if (!(p = strstr(name, ".ram")))
            return;

    int sav_slot;
    if ((int)lParam == -1)
        sav_slot = ((key_state['1']) ? 1 : ((key_state['2']) ? 2 : ((key_state['3']) ? 3 : ((key_state['4']) ? 4 : ((key_state['5']) ? 5 : ((key_state['6']) ? 6 : ((key_state['7']) ? 7 : ((key_state['8']) ? 8 : ((key_state['9']) ? 9 : 0)))))))));
    else
        sav_slot = (int)lParam;

    sprintf(p, ".sv%d", sav_slot);

    FILE *file = fopen(name, "rb");

    if (file) {
        g_gb->restore_state(file);
        int tmp;
        fseek(file, -100, SEEK_CUR);
        fread(&tmp, 4, 1, file);
        render->set_timer_state(tmp);
        fclose(file);
    }

    SetCurrentDirectory(cur_di);
}
