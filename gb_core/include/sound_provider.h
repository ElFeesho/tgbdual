#pragma once

class sound_provider {
public:
    virtual void populate_audio_buffer(short *buf, int samples) = 0;
};