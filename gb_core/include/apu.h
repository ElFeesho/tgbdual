#pragma once

#include <stdint.h>

#include "sound_provider.h"

class gb;

class serializer;

struct apu_stat {
    bool sq1_playing{false};
    int32_t sq1_sw_time{0};
    int32_t sq1_sw_dir{0};
    int32_t sq1_sw_shift{0};

    int32_t sq1_len{0};
    int32_t sq1_init_len{0};
    int32_t sq1_type{0};

    int32_t sq1_vol{0};
    int32_t sq1_init_vol{0};
    int32_t sq1_env_dir{0};
    int32_t sq1_env_speed{0};

    int32_t sq1_freq{0};
    int32_t sq1_init_freq{0};

    int32_t sq1_hold{0};

    bool sq2_playing{false};

    int32_t sq2_len{0};
    int32_t sq2_init_len{0};
    int32_t sq2_type{0};

    int32_t sq2_vol{0};
    int32_t sq2_init_vol{0};
    int32_t sq2_env_dir{0};
    int32_t sq2_env_speed{0};

    int32_t sq2_freq{0};
    int32_t sq2_init_freq{0};

    int32_t sq2_hold{0};

    bool wav_playing{false};
    int32_t wav_vol{0};
    int32_t wav_freq{0};
    int32_t wav_init_len{0};
    int32_t wav_len{0};
    int32_t wav_hold{0};

    bool noi_playing{false};
    int32_t noi_len{0};
    int32_t noi_init_len{0};

    int32_t noi_vol{0};
    int32_t noi_init_vol{0};
    int32_t noi_env_dir{0};
    int32_t noi_env_speed{0};

    int32_t noi_freq{0};
    int32_t noi_init_freq{0};
    int32_t noi_hold{0};
    int32_t noi_step{0};

    int32_t master_enable{1};
    int32_t ch_enable[4][2]{};
    int32_t master_vol[2]{0};
    int32_t ch_on[4]{0};
    int32_t wav_enable{0};
};

struct apu_que {
    uint16_t adr{0};
    uint8_t dat{0};
    int clock{0};
};

class apu;

class apu_snd : public sound_provider {
    friend class apu;

public:
    explicit apu_snd(apu *papu);

    void populate_audio_buffer(short *buf, int sample) override;

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
    int que_count{0};
    int bef_clock{0};
    apu *ref_apu{nullptr};

    bool b_echo{false};
    bool b_lowpass{false};

    uint8_t mem[0x100]{0};
    bool b_enable[4]{true};
};

class apu {
    friend class apu_snd;

public:
    explicit apu(gb &ref);

    apu_snd *get_stream_provider() { return &snd; }

    uint8_t read(uint16_t adr);

    void write(uint16_t adr, uint8_t dat, int clock);

    void reset();

    void serialize(serializer &s);

private:
    gb &ref_gb;
    apu_snd snd;
};
