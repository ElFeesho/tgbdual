#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <map>
#include <functional>

static unsigned int fromHexToInt(const std::string &hexString) {
    unsigned int val;
    std::stringstream ss;
    ss << std::hex << hexString;
    ss >> val;
    return val;
}

static uint16_t extractAddress(const std::string &cheatString)
{
    uint16_t adr = fromHexToInt(cheatString.substr(4));
    adr = ((adr << 8) & 0xff00) | ((adr >> 8) & 0x00ff);
    return adr;
}

static uint8_t extractData(const std::string &cheatString)
{
    return fromHexToInt(cheatString.substr(2, 2));   
}

static uint8_t extractCode(const std::string &cheatString)
{
    return fromHexToInt(cheatString.substr(0, 2));   
}

class gb;

class cheat_dat {
   public:
    cheat_dat(const std::string &cheat_code) {
        code = extractCode(cheat_code);
        dat = extractData(cheat_code);
        adr = extractAddress(cheat_code);
    }

    uint8_t code;
    uint16_t adr;
    uint8_t dat;
};

class cheat {
    using cpu_writecb = std::function<void(uint16_t, uint8_t)>;
   public:

    uint8_t cheat_read(uint8_t ram_bank_num, uint16_t adr, uint8_t or_value);

    void add_cheat(const std::string &code, cpu_writecb writecb);
    
    void clear();

   private:
    std::map<uint16_t, cheat_dat> cheat_map;
};
