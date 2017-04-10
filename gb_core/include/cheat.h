#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <map>
#include <functional>

class gb;

struct cheat_dat {
public:
    cheat_dat(const std::string &cheat_code);

    uint8_t code;
    uint16_t adr;
    uint8_t dat;
};

class cheat {
    using cpu_writecb = std::function<void(uint16_t, uint8_t)>;
public:

    uint8_t cheat_read(uint8_t ram_bank_num, uint16_t adr, uint8_t or_value);

    void add_cheat(const std::string &code, cpu_writecb writecb);

private:
    std::map<uint16_t, cheat_dat> cheat_map;
};
