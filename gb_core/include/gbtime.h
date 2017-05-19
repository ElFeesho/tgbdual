#pragma once


class gbtime {

public:
    void set_time(int type, uint8_t dat);

    uint8_t get_time(int type);

private:
    uint32_t _cur_time{0};
};


