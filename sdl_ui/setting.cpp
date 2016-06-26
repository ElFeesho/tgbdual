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

#include "setting.h"
#include "resource.h"

#include <stdio.h>
#include <string.h>

#include <SDL.h>

#include "dinput_sdl.h"
#include "w32_posix.h"

namespace {
int GetPrivateProfileInt(const char *opt, const char *key, int def, Resource &rc) {
    std::string k = std::string(opt) + ":" + key;
    if (rc.has(k)) {
        return rc.get_int(k);
    } else {
        return def;
    }
}
void GetPrivateProfileString(const char *opt, const char *key, const char *def, char *dst, int len, Resource &rc) {
    std::string k = std::string(opt) + ":" + key;
    if (rc.has(k)) {
        std::string val = rc.get_string(k);
        strncpy(dst, val.c_str(), len);
    } else {
        strncpy(dst, def, len);
    }
}

void trans_key(int *key) {
    if (key[0] == 1) {
        key[1] = DIK_keymap[key[1]];
    }
}
}

setting::setting() {
    DX5_InitOSKeymap();

    GetCurrentDirectory(256, home_dir);

    Resource rc;

    FILE *fp = fopen("TGB.ini", "r");
    if (fp) {
        fclose(fp);
        printf("loading TGB.ini\n");
        rc.addResourceFile("TGB.ini");
    } else {
        printf("Warning: TGB.ini not found, using default.\n");
    }

    // キー情報 (a,b,select,start,down,up,left,right の順)
    key_setting[0][0] = GetPrivateProfileInt("key_1", "a_type", 1, rc);
    key_setting[0][1] = GetPrivateProfileInt("key_1", "a_code", DIK_Z, rc);
    key_setting[0][2] = GetPrivateProfileInt("key_1", "b_type", 1, rc);
    key_setting[0][3] = GetPrivateProfileInt("key_1", "b_code", DIK_X, rc);
    key_setting[0][4] = GetPrivateProfileInt("key_1", "select_type", 1, rc);
    key_setting[0][5] = GetPrivateProfileInt("key_1", "select_code", DIK_C, rc);
    key_setting[0][6] = GetPrivateProfileInt("key_1", "start_type", 1, rc);
    key_setting[0][7] = GetPrivateProfileInt("key_1", "start_code", DIK_V, rc);
    key_setting[0][8] = GetPrivateProfileInt("key_1", "down_type", 1, rc);
    key_setting[0][9] = GetPrivateProfileInt("key_1", "down_code", DIK_DOWN, rc);
    key_setting[0][10] = GetPrivateProfileInt("key_1", "up_type", 1, rc);
    key_setting[0][11] = GetPrivateProfileInt("key_1", "up_code", DIK_UP, rc);
    key_setting[0][12] = GetPrivateProfileInt("key_1", "left_type", 1, rc);
    key_setting[0][13] = GetPrivateProfileInt("key_1", "left_code", DIK_LEFT, rc);
    key_setting[0][14] = GetPrivateProfileInt("key_1", "right_type", 1, rc);
    key_setting[0][15] = GetPrivateProfileInt("key_1", "right_code", DIK_RIGHT, rc);

    key_setting[1][0] = GetPrivateProfileInt("key_2", "a_type", 1, rc);
    key_setting[1][1] = GetPrivateProfileInt("key_2", "a_code", DIK_DELETE, rc);
    key_setting[1][2] = GetPrivateProfileInt("key_2", "b_type", 1, rc);
    key_setting[1][3] = GetPrivateProfileInt("key_2", "b_code", DIK_END, rc);
    key_setting[1][4] = GetPrivateProfileInt("key_2", "select_type", 1, rc);
    key_setting[1][5] = GetPrivateProfileInt("key_2", "select_code", DIK_HOME, rc);
    key_setting[1][6] = GetPrivateProfileInt("key_2", "start_type", 1, rc);
    key_setting[1][7] = GetPrivateProfileInt("key_2", "start_code", DIK_INSERT, rc);
    key_setting[1][8] = GetPrivateProfileInt("key_2", "down_type", 1, rc);
    key_setting[1][9] = GetPrivateProfileInt("key_2", "down_code", DIK_NUMPAD2, rc);
    key_setting[1][10] = GetPrivateProfileInt("key_2", "up_type", 1, rc);
    key_setting[1][11] = GetPrivateProfileInt("key_2", "up_code", DIK_NUMPAD8, rc);
    key_setting[1][12] = GetPrivateProfileInt("key_2", "left_type", 1, rc);
    key_setting[1][13] = GetPrivateProfileInt("key_2", "left_code", DIK_NUMPAD4, rc);
    key_setting[1][14] = GetPrivateProfileInt("key_2", "right_type", 1, rc);
    key_setting[1][15] = GetPrivateProfileInt("key_2", "right_code", DIK_NUMPAD6, rc);

    // システムキー
    fast_forwerd[0] = GetPrivateProfileInt("sys_key", "fast_type", 1, rc);
    fast_forwerd[1] = GetPrivateProfileInt("sys_key", "fast_code", DIK_TAB, rc);
    save_key[0] = GetPrivateProfileInt("sys_key", "save_type", 1, rc);
    save_key[1] = GetPrivateProfileInt("sys_key", "save_code", DIK_F5, rc);
    load_key[0] = GetPrivateProfileInt("sys_key", "load_type", 1, rc);
    load_key[1] = GetPrivateProfileInt("sys_key", "load_code", DIK_F7, rc);
    auto_key[0] = GetPrivateProfileInt("sys_key", "auto_type", 1, rc);
    auto_key[1] = GetPrivateProfileInt("sys_key", "auto_code", DIK_A, rc);
    pause_key[0] = GetPrivateProfileInt("sys_key", "pause_type", 1, rc);
    pause_key[1] = GetPrivateProfileInt("sys_key", "pause_code", DIK_P, rc);
    full_key[0] = GetPrivateProfileInt("sys_key", "full_type", 1, rc);
    full_key[1] = GetPrivateProfileInt("sys_key", "full_code", DIK_F, rc);
    reset_key[0] = GetPrivateProfileInt("sys_key", "reset_type", 1, rc);
    reset_key[1] = GetPrivateProfileInt("sys_key", "reset_code", DIK_R, rc);
    quit_key[0] = GetPrivateProfileInt("sys_key", "quit_type", 1, rc);
    quit_key[1] = GetPrivateProfileInt("sys_key", "quit_code", DIK_ESCAPE, rc);

    // コロコロカービィ
    koro_use_analog = GetPrivateProfileInt("korokoro", "use_analog", 0, rc) ? true : false;
    koro_sensitive = GetPrivateProfileInt("korokoro", "sensitivity", 100, rc);
    koro_key[0] = GetPrivateProfileInt("korokoro", "up_type", 1, rc);
    koro_key[1] = GetPrivateProfileInt("korokoro", "up_code", DIK_NUMPAD8, rc);
    koro_key[2] = GetPrivateProfileInt("korokoro", "down_type", 1, rc);
    koro_key[3] = GetPrivateProfileInt("korokoro", "down_code", DIK_NUMPAD2, rc);
    koro_key[4] = GetPrivateProfileInt("korokoro", "left_type", 1, rc);
    koro_key[5] = GetPrivateProfileInt("korokoro", "left_code", DIK_NUMPAD4, rc);
    koro_key[6] = GetPrivateProfileInt("korokoro", "right_type", 1, rc);
    koro_key[7] = GetPrivateProfileInt("korokoro", "right_code", DIK_NUMPAD6, rc);

    // その他
    use_ffb = GetPrivateProfileInt("special", "use_ffb", 0, rc) ? true : false;
    gb_type = GetPrivateProfileInt("special", "gb_type", 0, rc);
    use_gba = GetPrivateProfileInt("special", "use_gba", 0, rc) ? true : false;
    render_pass = GetPrivateProfileInt("special", "render_pass", 2, rc);
    priority_class = GetPrivateProfileInt("special", "priority_class", 3, rc);

    // 速度
    frame_skip = GetPrivateProfileInt("speed", "normal_frame_skip", 1, rc);
    virtual_fps = GetPrivateProfileInt("speed", "normal_fps", 60, rc);
    speed_limit = (GetPrivateProfileInt("speed", "normal_limit", 1, rc)) ? true : false;
    fast_frame_skip = GetPrivateProfileInt("speed", "fast_frame_skip", 9, rc);
    fast_virtual_fps = GetPrivateProfileInt("speed", "fast_fps", 9999, rc);
    fast_speed_limit = (GetPrivateProfileInt("speed", "fast_limit", 0, rc)) ? true : false;
    show_fps = (GetPrivateProfileInt("speed", "show_fps", 0, rc)) ? true : false;

    // カラーフィルタ
    r_def = GetPrivateProfileInt("filter", "r_def", 0, rc);
    g_def = GetPrivateProfileInt("filter", "g_def", 0, rc);
    b_def = GetPrivateProfileInt("filter", "b_def", 0, rc);
    r_div = GetPrivateProfileInt("filter", "r_div", 256, rc);
    g_div = GetPrivateProfileInt("filter", "g_div", 256, rc);
    b_div = GetPrivateProfileInt("filter", "b_div", 256, rc);
    r_r = GetPrivateProfileInt("filter", "r_r", 256, rc);
    r_g = GetPrivateProfileInt("filter", "r_g", 0, rc);
    r_b = GetPrivateProfileInt("filter", "r_b", 0, rc);
    g_r = GetPrivateProfileInt("filter", "g_r", 0, rc);
    g_g = GetPrivateProfileInt("filter", "g_g", 256, rc);
    g_b = GetPrivateProfileInt("filter", "g_b", 0, rc);
    b_r = GetPrivateProfileInt("filter", "b_r", 0, rc);
    b_g = GetPrivateProfileInt("filter", "b_g", 0, rc);
    b_b = GetPrivateProfileInt("filter", "b_b", 256, rc);

    // サウンド
    sound_enable[0] = GetPrivateProfileInt("sound", "sq_wav1", 1, rc);
    sound_enable[1] = GetPrivateProfileInt("sound", "sq_wav2", 1, rc);
    sound_enable[2] = GetPrivateProfileInt("sound", "sq_voluntary", 1, rc);
    sound_enable[3] = GetPrivateProfileInt("sound", "sq_noise", 1, rc);
    sound_enable[4] = GetPrivateProfileInt("sound", "master", 1, rc);
    b_echo = GetPrivateProfileInt("sound", "echo", 0, rc) ? true : false;
    b_lowpass = GetPrivateProfileInt("sound", "lowpass_filter", 1, rc) ? true : false;

    // IP アドレス
    GetPrivateProfileString("ip_addr", "addr_1", "", ip_addrs[0], 20, rc);
    GetPrivateProfileString("ip_addr", "addr_2", "", ip_addrs[1], 20, rc);
    GetPrivateProfileString("ip_addr", "addr_3", "", ip_addrs[2], 20, rc);
    GetPrivateProfileString("ip_addr", "addr_4", "", ip_addrs[3], 20, rc);

    if (!GetPrivateProfileInt("special", "no_dinput", 0, rc)) {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 16; j += 2) {
                trans_key(&(key_setting[i][j]));
            }
        }
        trans_key(save_key);
        trans_key(load_key);
        trans_key(auto_key);
        trans_key(fast_forwerd);
        trans_key(pause_key);
        trans_key(full_key);
        trans_key(reset_key);
        trans_key(quit_key);
        for (int i = 0; i < 8; i += 2) {
            trans_key(&(koro_key[i]));
        }
    }

    // ディレクトリ
    char tmp_save[256];

    strcpy(tmp_save, home_dir);
    strcat(tmp_save, "/save");

    GetPrivateProfileString("directory", "save_dir", tmp_save, save_dir, 256, rc);

    if (strchr(save_dir, ':')) {
        strcpy(save_dir, getenv("HOME"));
        strcat(save_dir, "/.tgbdual");
        CreateDirectory(save_dir, NULL);
        strcat(save_dir, "/save");
        printf("Determined save dir to be %s\n", save_dir);
    }

    if (SetCurrentDirectory(save_dir)) {
        CreateDirectory(save_dir, NULL);
    }
    SetCurrentDirectory(home_dir);
}

setting::~setting() {
    char ini_name[256];
    strcpy(ini_name, home_dir);
    strcat(ini_name, "/TGB.ini");
}

void setting::get_key_setting(int *buf, int side) {
    memcpy(buf, key_setting[side], sizeof(int) * 16);
}

void setting::set_key_setting(int *dat, int side) {
    memcpy(key_setting[side], dat, sizeof(int) * 16);
}

void setting::get_save_dir(char *buf) {
    strcpy(buf, save_dir);
}

void setting::set_save_dir(char *dat) {
    strcpy(save_dir, dat);
}

void setting::get_media_dir(char *buf) {
    strcpy(buf, media_dir);
}

void setting::set_media_dir(char *dat) {
    strcpy(media_dir, dat);
}

void setting::get_dev_dir(char *buf) {
    strcpy(buf, dev_dir);
}

void setting::get_home_dir(char *buf) {
    strcpy(buf, home_dir);
}
