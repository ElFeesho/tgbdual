#pragma once

#include "sound_renderer.h"

class gb;
class serializer;

struct apu_stat {
    bool sq1_playing;
    int sq1_sw_time;
    int sq1_sw_dir;
    int sq1_sw_shift;

    int sq1_len;
    int sq1_init_len;
    int sq1_type;

    int sq1_vol;
    int sq1_init_vol;
    int sq1_env_dir;
    int sq1_env_speed;

    int sq1_freq;
    int sq1_init_freq;

    int sq1_hold;

    bool sq2_playing;

    int sq2_len;
    int sq2_init_len;
    int sq2_type;

    int sq2_vol;
    int sq2_init_vol;
    int sq2_env_dir;
    int sq2_env_speed;

    int sq2_freq;
    int sq2_init_freq;

    int sq2_hold;

    bool wav_playing;
    int wav_vol;
    int wav_freq;
    int wav_init_freq;
    int wav_init_len;
    int wav_len;
    int wav_hold;

    bool noi_playing;
    int noi_len;
    int noi_init_len;

    int noi_vol;
    int noi_init_vol;
    int noi_env_dir;
    int noi_env_speed;

    int noi_freq;
    int noi_init_freq;
    int noi_hold;
    int noi_step;

    int master_enable;
    int ch_enable[4][2];
    int master_vol[2];
    int ch_on[4];
    int wav_enable;
};

struct apu_que {
    uint16_t adr;
    uint8_t dat;
    int clock;
};

class apu;

class apu_snd : public sound_renderer {
    friend class apu;

   public:
    apu_snd(apu *papu);

    void set_enable(int ch, bool enable);
    bool get_enable(int ch);
    void set_echo(bool echo) { b_echo = echo; };
    void set_lowpass(bool lowpass) { b_lowpass = lowpass; };
    bool get_echo() { return b_echo; };
    bool get_lowpass() { return b_lowpass; };

    void render(short *buf, int sample);
    void reset();

    void serialize(serializer &s);

   private:
    void process(uint16_t adr, uint8_t dat);
    void update();
    short sq1_produce(int freq);
    short sq2_produce(int freq);
    short wav_produce(int freq, bool interpolation);
    short noi_produce(int freq);

    apu_stat stat;
    apu_stat stat_cpy, stat_tmp;
    apu_que write_que[0x10000];
    int que_count;
    int bef_clock;
    apu *ref_apu;

    bool b_echo;
    bool b_lowpass;

    uint8_t mem[0x100];
    bool b_enable[4];
};

class apu {
    friend class apu_snd;

   public:
    apu(gb *ref);

    apu_snd *get_renderer() { return &snd; }
    apu_stat *get_stat();
    apu_stat *get_stat_cpy();
    uint8_t *get_mem();

    uint8_t read(uint16_t adr);
    void write(uint16_t adr, uint8_t dat, int clock);

    void update();
    void reset();

    void serialize(serializer &s);

   private:
    gb *ref_gb;
    apu_snd snd;
};

