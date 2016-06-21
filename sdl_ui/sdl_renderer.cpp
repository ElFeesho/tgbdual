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

//------------------------------------------------------
// interface renderer の SDLを用いた実装
// Using SDL implementation of interface renderer

#include "sdl_renderer.h"
#include "resource.h"
#include <stdio.h>
#include <stdarg.h>

#include "w32_posix.h"
#include "callbacks.h"
#include "setting.h"

#include "../gb_core/gb.h"

extern gb *g_gb[2];
extern setting *config;
extern Uint8 *key_state;

sdl_renderer::sdl_renderer() {
    b_window = true;
    render_pass_type = 2;

    init_sdlvideo();
    init_sdlaudio();
    init_sdlevent();

    cur_time = 0;
    now_sensor_x = now_sensor_y = 2047;
    b_pad_update = true;

    save_resurve = -1;
    load_resurve = -1;

    b_sound = b_graphics = false;
    b_bibrating = false;

    key_list.clear();
    movie_playing = movie_recording = false;
    movie_start = mov_cur_pos = 0;
    movie_file = NULL;
}

sdl_renderer::~sdl_renderer() {
    uninit_sdlvideo();
    uninit_sdlaudio();
    uninit_sdlevent();
}

void sdl_renderer::graphics_record(char *file) {
    strcpy(graphics_file, file);
    b_graphics = true;
}

void sdl_renderer::sound_record(char *file) {
    
}

void sdl_renderer::output_log(char *mes, ...) {
    va_list vl;
    char buf[256];

    va_start(vl, mes);
    vsprintf(buf, mes, vl);

    printf("%s\n", buf);
    
    va_end(vl);
    return;
}

word sdl_renderer::map_color(word gb_col) {
#define convert
#ifdef convert
    word r, g, b;
    int r2, g2, b2;

    r = ((gb_col >> 0) & 0x1f) << 3;
    g = ((gb_col >> 5) & 0x1f) << 3;
    b = ((gb_col >> 10) & 0x1f) << 3;

    r2 = m_filter.r_def + ((r * m_filter.r_r + g * m_filter.r_g + b * m_filter.r_b) / ((!m_filter.r_div) ? 1 : m_filter.r_div));
    g2 = m_filter.g_def + ((r * m_filter.g_r + g * m_filter.g_g + b * m_filter.g_b) / ((!m_filter.g_div) ? 1 : m_filter.g_div));
    b2 = m_filter.b_def + ((r * m_filter.b_r + g * m_filter.b_g + b * m_filter.b_b) / ((!m_filter.b_div) ? 1 : m_filter.b_div));

    r2 = (r2 > 255) ? 255 : ((r2 < 0) ? 0 : r2);
    g2 = (g2 > 255) ? 255 : ((g2 < 0) ? 0 : g2);
    b2 = (b2 > 255) ? 255 : ((b2 < 0) ? 0 : b2);

    gb_col = (r2 >> 3) | ((g2 >> 3) << 5) | ((b2 >> 3) << 10);

#endif
    // xBBBBBGG GGGRRRRR から変換 // converted from xBBBBBGG GGGRRRRR
    if (color_type == 0) // ->RRRRRGGG GGxBBBBB に変換 (565) // converted to
        return ((gb_col & 0x1F) << 11) | ((gb_col & 0x3e0) << 1) | ((gb_col & 0x7c00) >> 10) | ((gb_col & 0x8000) >> 10);
    if (color_type == 1) // ->xRRRRRGG GGGBBBBB に変換 (1555) // converted to
        return ((gb_col & 0x1F) << 10) | (gb_col & 0x3e0) | ((gb_col & 0x7c00) >> 10) | (gb_col & 0x8000);
    if (color_type == 2) // ->RRRRRGGG GGBBBBBx に変換 (5551) // converted to
        return ((gb_col & 0x1F) << 11) | ((gb_col & 0x3e0) << 1) | ((gb_col & 0x7c00) >> 9) | (gb_col >> 15);
    else
        return gb_col;
}

word sdl_renderer::unmap_color(word gb_col) {
    // xBBBBBGG GGGRRRRR へ変換 // converted to xBBBBBGG GGGRRRRR
    if (color_type == 0) // ->RRRRRGGG GGxBBBBB から変換 (565) // convert from
        return (gb_col >> 11) | ((gb_col & 0x7c0) >> 1) | (gb_col << 10) | ((gb_col & 0x40) << 10);
    if (color_type == 1) // ->xRRRRRGG GGGBBBBB から変換 (1555) // convert from
        return ((gb_col & 0x7c00) >> 10) | (gb_col & 0x3e0) | ((gb_col & 0x1f) << 10) | (gb_col & 0x8000);
    if (color_type == 2) // ->RRRRRGGG GGBBBBBx から変換 (5551) // convert from
        return (gb_col >> 11) | ((gb_col & 0x7c0) >> 1) | ((gb_col & 0x3e) << 9) | (gb_col << 15);
    else
        return gb_col;
}

static dword convert_to_second(SYSTEMTIME *sys) {
    dword i, ret = 0;
    static int month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    for (i = 1; i + 1950 < sys->tm_year; i++)
        if ((i & 3) == 0)           // 閏年 // Leap year
            if ((i % 100) == 0)     // 閏年例外 // Leap year exception
                if ((i % 400) == 0) // やっぱ閏年 // Leap year after all
                    ret += 366;
                else
                    ret += 365;
            else
                ret += 366;
        else
            ret += 365;

    for (i = 1; i < sys->tm_mon; i++)
        if (i == 2)
            if ((sys->tm_year & 3) == 0)
                if ((sys->tm_year % 100) == 0)
                    if ((sys->tm_year % 400) == 0)
                        ret += 29;
                    else
                        ret += 28;
                else
                    ret += 29;
            else
                ret += 28;
        else
            ret += month_days[i];

    /// is it ok?
    ret += sys->tm_mday - 1;

    ret *= 24 * 60 * 60; // 秒に変換 // Converted to seconds

    ret += sys->tm_hour * 60 * 60;
    ret += sys->tm_min * 60;
    ret += sys->tm_sec;

    return ret;
}

static int pathed_seconds(SYSTEMTIME from, SYSTEMTIME to) {
    return convert_to_second(&to) - convert_to_second(&from);
}

byte sdl_renderer::get_time(int type) {
    SYSTEMTIME sys;
    GetSystemTime(&sys);

    dword now = convert_to_second(&sys);
    now -= cur_time;

    switch (type) {
        case 8: // 秒 // Second
            return (byte)(now % 60);
        case 9: // 分 // Minute
            return (byte)((now / 60) % 60);
        case 10: // 時 // Hour
            return (byte)((now / (60 * 60)) % 24);
        case 11: // 日(L) // Day (L)
            return (byte)((now / (24 * 60 * 60)) & 0xff);
        case 12: // 日(H) // Day (H)
            return (byte)((now / (256 * 24 * 60 * 60)) & 1);
    }
    return 0;
}

void sdl_renderer::set_time(int type, byte dat) {
    SYSTEMTIME sys;
    GetSystemTime(&sys);

    dword now = convert_to_second(&sys);
    dword adj = now - cur_time;

    switch (type) {
        case 8: // 秒 // Second
            adj = (adj / 60) * 60 + (dat % 60);
            break;
        case 9: // 分 // Minute
            adj = (adj / (60 * 60)) * 60 * 60 + (dat % 60) * 60 + (adj % 60);
            break;
        case 10: // 時 // Hour
            adj = (adj / (24 * 60 * 60)) * 24 * 60 * 60 + (dat % 24) * 60 * 60 + (adj % (60 * 60));
            break;
        case 11: // 日(L) // Day (L)
            adj = (adj / (256 * 24 * 60 * 60)) * 256 * 24 * 60 * 60 + (dat * 24 * 60 * 60) + (adj % (24 * 60 * 60));
            break;
        case 12: // 日(H) // Day (H)
            adj = (dat & 1) * 256 * 24 * 60 * 60 + (adj % (256 * 24 * 60 * 60));
            break;
    }
    cur_time = now - adj;
}

int sdl_renderer::get_timer_state() {
    return cur_time;
}

void sdl_renderer::set_timer_state(int timer) {
    cur_time = timer;
}

word sdl_renderer::get_sensor(bool x_y) {
    return (x_y ? (now_sensor_x & 0x0fff) : (now_sensor_y & 0x0fff));
}

//------------------------------------------------------------

void sdl_renderer::init_sdlvideo() {
    printf("init video\n");

    init_surface();
    mes_show = false;
    mes[0] = '\0';
    mul = 1;
}

void sdl_renderer::swith_screen_mode() {
    release_surface();
    b_window = !b_window;
    init_surface();
}

void sdl_renderer::set_render_pass(int type) {
    if ((type >= 0) && (type <= 2) && (render_pass_type != type)) {
        render_pass_type = type;
        release_surface();
        init_surface();
    }
}

void sdl_renderer::init_surface() {
    int w = 160;
    int h = 144;
    static const int GB_W = 160;
    static const int GB_H = 144;
    Uint32 flags = SDL_SWSURFACE;
    if (!b_window)
        flags |= SDL_FULLSCREEN;

    if (w != GB_W || h != GB_H) {
        dpy = SDL_SetVideoMode(w, h, 16, flags);
        SDL_Surface *tmp =
            SDL_CreateRGBSurface(SDL_SWSURFACE, GB_W, GB_H, 16, 0, 0, 0, 0);
        scr = SDL_DisplayFormat(tmp);
        SDL_FreeSurface(tmp);
    } else {
        dpy = 0;
        scr = SDL_SetVideoMode(GB_W, GB_H, 16, flags);
    }
}

void sdl_renderer::uninit_sdlvideo() {
    release_surface();
}

void sdl_renderer::release_surface() {
    if (dpy) {
        SDL_FreeSurface(scr);
    }
}

void sdl_renderer::on_move() {
}

void sdl_renderer::draw_menu(int n) {
}

void sdl_renderer::render_screen(byte *buf, int width, int height, int depth) {
    Uint64 *sp = (Uint64 *)buf;
    Uint64 *dp = (Uint64 *)scr->pixels;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < (width >> 2); j++) {
            // xBBBBBGGGGGRRRRR => RRRRRGGGGGxBBBBB
            *dp = *sp;
            // ((*sp & 0x7c007c007c007c00ll) >> 10) |
            //     ((*sp & 0x03e003e003e003e0ll) << 1) |
            //     ((*sp & 0x001f001f001f001fll) << 11);
            dp++;
            sp++;
        }
    }

    flip();
}

void sdl_renderer::flip() {
    static long beforetime = timeGetTime();
    static int count = 0, beforecount = 0;
    char buf[256];

    sprintf(buf, "%4d  ", beforecount * mul);

    if (timeGetTime() - beforetime > 1000) {
        beforecount = count;
        count = 0;
        beforetime = timeGetTime();

        if (b_showfps) {
            printf("%sFPS.\n", buf);
        }
    }

    if (dpy)
        SDL_UpdateRect(dpy, 0, 0, 0, 0);
    else
        SDL_UpdateRect(scr, 0, 0, 0, 0);

    count++;
}

void sdl_renderer::show_message(const char *message) {
    printf("%s\n", message);
}

//----------------------------------------------

#define BUF_LEN 160

namespace {
void fill_audio(void *, Uint8 *stream, int len) {
    if (g_gb[0]) {
        apu_snd *snd_render = g_gb[0]->get_apu()->get_renderer();
        snd_render->render((short *)stream, len / 4);
    }
}
}

void sdl_renderer::init_sdlaudio() {
    printf("init audio\n");

    SDL_AudioSpec wanted;

    wanted.freq = 44100;
    wanted.format = AUDIO_S16;
    wanted.channels = 2; /* 1 = モノラル, 2 = ステレオ */ /* 1 = mono, 2 = stereo */
    wanted.samples = 4096; /* 遅延も少なく推奨の値です */ /* Recommended value less delay */
    wanted.callback = fill_audio;
    wanted.userdata = NULL;

    if (SDL_OpenAudio(&wanted, NULL) < 0) {
        fprintf(stderr, "Could not open audio device: %s\n", SDL_GetError());
    }

    SDL_PauseAudio(0);
}

void sdl_renderer::uninit_sdlaudio() {
    SDL_PauseAudio(1);
}

void sdl_renderer::pause_sound() {
    SDL_PauseAudio(1);
}

void sdl_renderer::resume_sound() {
    SDL_PauseAudio(0);
}

void sdl_renderer::init_sdlevent() {
    key_state = SDL_GetKeyState(0);

    pad_state = 0;
    memset(&key_config, 0, sizeof(key_config));
    bool b_auto = false;
}

void sdl_renderer::uninit_sdlevent() {
    /**
	m_pkeyboad->Unacquire();
	m_pkeyboad->Release();

	if (b_can_use_ffb)
		m_peffect->Release();

	for (int i=0;i<joysticks;i++){
		m_pjoystick2[i]->Release();
		m_pjoystick[i]->Unacquire();
		m_pjoystick[i]->Release();
	}
	m_pdi->Release();
*/
}

void sdl_renderer::set_key(key_dat *keys) {
    memcpy(key_config, keys, sizeof(key_dat) * 8);
}

void sdl_renderer::set_koro_key(key_dat *keys) {
    memcpy(koro_key_config, keys, sizeof(key_dat) * 4);
}

void sdl_renderer::set_koro_analog(bool ana) {
    b_koro_analog = ana;
}

void sdl_renderer::set_koro_sensitivity(int sence) {
    koro_sence = sence;
}

void sdl_renderer::set_bibrate(bool bibrate) {
    /** たぶん実装できない ** probably can not be implemented
	b_bibrating=bibrate;
	if (!(b_use_ffb&&b_can_use_ffb))
		return;

	if (bibrate){
		m_peffect->Start(1,0);
		LONG rglDirection[2] = { 0, 0 };

	    DICONSTANTFORCE cf;
	    cf.lMagnitude = 10000;

		DIEFFECT eff;
		ZeroMemory( &eff, sizeof(eff) );
		eff.dwSize                = sizeof(DIEFFECT);
		eff.dwFlags               = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
		eff.cAxes                 = 2;
		eff.rglDirection          = rglDirection;
		eff.lpEnvelope            = 0;
		eff.cbTypeSpecificParams  = sizeof(DICONSTANTFORCE);
		eff.lpvTypeSpecificParams = &cf;
//		eff.dwStartDelay          = 0;

		// Now set the new parameters and start the effect immediately.
		m_peffect->SetParameters(&eff,DIEP_DIRECTION|DIEP_TYPESPECIFICPARAMS|DIEP_START);
	}
	else
		m_peffect->Stop();
*/
}

void sdl_renderer::set_use_ffb(bool use) {
    b_use_ffb = use;
}

int sdl_renderer::check_pad() {
    return pad_state;
}

void sdl_renderer::set_pad(int stat) {
    pad_state = stat;
}

void sdl_renderer::disable_check_pad() {
    b_pad_update = false;
}

void sdl_renderer::enable_check_pad() {
    b_pad_update = true;
}

void sdl_renderer::toggle_auto() {
    b_auto = !b_auto;
    show_message(b_auto ? "auto fire enabled" : "auto fire disabled");
}

void sdl_renderer::movie_play_start(vector<mov_key> *list) {
    movie_playing = true;
    movie_start = 0;
    mov_cur_pos = 0;

    key_list.clear();
    vector<mov_key>::iterator ite;
    for (ite = list->begin(); ite != list->end(); ite++)
        key_list.push_back(*ite);
}

void sdl_renderer::update_pad() {
    SDL_PumpEvents();
    key_state = SDL_GetKeyState(0);

    static bool phase = false;

    pad_state = 0;

    if (b_auto) {
        if (phase) {
            for (int i = 0; i < 2; i++)
                pad_state |= check_press(key_config + i) ? (1 << i) : 0;
        }
        for (int i = 2; i < 8; i++)
            pad_state |= check_press(key_config + i) ? (1 << i) : 0;
    } else
        for (int i = 0; i < 8; i++)
            pad_state |= check_press(key_config + i) ? (1 << i) : 0;

    phase = !phase;
}

word sdl_renderer::get_any_key() {
    return 0;
}

bool sdl_renderer::check_press(key_dat *dat) {
    int pad_id, pad_dir;

    enum { JW = 8192 };

    if (dat->device_type == DI_KEYBOARD) { // キーボード
        return (key_state[dat->key_code]) ? true : false;
    } else if (dat->device_type >= DI_PAD_X) { // ジョイスティック
        pad_id = (dat->device_type - DI_PAD_X) / NEXT_PAD;
        pad_dir = (dat->device_type - DI_PAD_X) % NEXT_PAD;
        SDL_Joystick *j = SDL_JoystickOpen(pad_id);
        if (!j)
            return false;
        if (pad_dir == 0) {     // X軸 // X axis
            if (!dat->key_code) // 正方向 // Positive direction
                return (SDL_JoystickGetAxis(j, 0) > JW) ? true : false;
            else // 負方向 // Negative direction
                return (SDL_JoystickGetAxis(j, 0) < -JW) ? true : false;
        } else if (pad_dir == 1) { // Y軸 // Y axis
            if (!dat->key_code)    // 正方向 // Positive direction
                return (SDL_JoystickGetAxis(j, 1) > JW) ? true : false;
            else // 負方向 // Negative direction
                return (SDL_JoystickGetAxis(j, 1) < -JW) ? true : false;
        } else if (pad_dir == 2) { // ボタン // Button
            return (SDL_JoystickGetButton(j, dat->key_code)) ? true : false;
        }
    }

    return false;
}

void sdl_renderer::refresh() {
    static bool bef_f5 = false, bef_f7 = false, bef_auto = false, bef_pause = false, bef_full = false, bef_reset = false, bef_quit = false;

    if (b_pad_update || movie_playing || movie_recording)
        update_pad();

    if ((!bef_f5 && check_press(&save_key)) || (save_resurve != -1)) {
        cb_save_state(save_resurve);
        save_resurve = -1;
    } else if ((!bef_f7 && check_press(&load_key)) || (load_resurve != -1)) {
        cb_load_state(load_resurve);
        load_resurve = -1;
    } else if (!bef_auto && check_press(&auto_key))
        toggle_auto();
    else if (!bef_pause && check_press(&pause_key))
        cb_pause(0);
    else if (!bef_full && check_press(&full_key))
        cb_fullscreen(0);
    else if (!bef_reset && check_press(&reset_key))
        cb_reset(0);
    else if (!bef_quit && check_press(&quit_key))
        cb_quit(0);
    bef_f5 = check_press(&save_key) ? true : false;
    bef_f7 = check_press(&load_key) ? true : false;
    bef_auto = check_press(&auto_key) ? true : false;
    bef_pause = check_press(&pause_key) ? true : false;
    bef_full = check_press(&full_key) ? true : false;
    bef_reset = check_press(&reset_key) ? true : false;
    bef_quit = check_press(&quit_key) ? true : false;
}
