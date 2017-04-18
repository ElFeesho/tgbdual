#pragma once

#include <stdint.h>

#include "sound_provider.h"

class gb;
class serializer;

struct apu_stat {
    bool sq1_playing { false };
    int32_t sq1_sw_time;
    int32_t sq1_sw_dir;
    int32_t sq1_sw_shift;

    int32_t sq1_len;
    int32_t sq1_init_len;
    int32_t sq1_type;

    int32_t sq1_vol;
    int32_t sq1_init_vol;
    int32_t sq1_env_dir;
    int32_t sq1_env_speed;

    int32_t sq1_freq;
    int32_t sq1_init_freq;

    int32_t sq1_hold;

    bool sq2_playing { false };

    int32_t sq2_len;
    int32_t sq2_init_len;
    int32_t sq2_type;

    int32_t sq2_vol;
    int32_t sq2_init_vol;
    int32_t sq2_env_dir;
    int32_t sq2_env_speed;

    int32_t sq2_freq;
    int32_t sq2_init_freq;

    int32_t sq2_hold;

    bool wav_playing { false };
    int32_t wav_vol;
    int32_t wav_freq;
    int32_t wav_init_len;
    int32_t wav_len;
    int32_t wav_hold;

    bool noi_playing { false };
    int32_t noi_len;
    int32_t noi_init_len;

    int32_t noi_vol;
    int32_t noi_init_vol;
    int32_t noi_env_dir;
    int32_t noi_env_speed;

    int32_t noi_freq;
    int32_t noi_init_freq;
    int32_t noi_hold;
    int32_t noi_step;

    int32_t master_enable { 1 };
    int32_t ch_enable[4][2];
    int32_t master_vol[2];
    int32_t ch_on[4];
    int32_t wav_enable;
};

struct apu_que {
    uint16_t adr;
    uint8_t dat;
    int clock;
};

class apu;

class apu_snd : public sound_provider {
    friend class apu;

public:
    apu_snd(apu *papu);

    void populate_audio_buffer(short *buf, int sample);
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

    apu_snd *get_stream_provider() { return &snd; }

    uint8_t read(uint16_t adr);
    void write(uint16_t adr, uint8_t dat, int clock);

    void reset();

    void serialize(serializer &s);

private:
    gb *ref_gb;
    apu_snd snd;
};
