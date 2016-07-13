#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <list>
#include <cstring>
#include <iostream>
#include <iomanip>

class gb;

class cheat_dat {
public:
    cheat_dat(const std::string &cheat_name, const std::string &cheat_code)
    {
        std::stringstream stream;
        stream << std::hex << std::setfill('0') << cheat_code.substr(0, 2);
        stream >> code;
        stream << std::hex << cheat_code.substr(2,2);
        stream >> dat;
        stream << std::hex << cheat_code.substr(4);
        stream >> adr;
        std::cout << "CODE " << cheat_code.substr(0, 2) << " " << std::hex << code << std::endl;
        std::cout << "DATA " << cheat_code.substr(2,2) << " " << std::hex << dat << std::endl;
        std::cout << "ADDR " << cheat_code.substr(4) << " " << std::hex << adr << std::endl;
        adr = __bswap_constant_16(adr);
        strcpy(name, cheat_name.c_str());
        enable = true;
        next = nullptr;
    }
    bool enable;
    uint8_t code;
    uint16_t adr;
    uint8_t dat;
    char name[255];
    cheat_dat *next;
};

class cheat {
public:
    cheat(gb *ref);
    ~cheat();

    uint8_t cheat_read(uint16_t adr);
    void cheat_write(uint16_t adr, uint8_t dat);

    bool cheak_cheat(uint16_t adr);
    void create_cheat_map();

    void add_cheat(cheat_dat *dat);
    void delete_cheat(char *name);
    std::list<cheat_dat>::iterator find_cheat(char *name);
    void create_unique_name(char *buf);

    void clear();

    std::list<cheat_dat>::iterator get_first() { return cheat_list.begin(); }
    std::list<cheat_dat>::iterator get_end() { return cheat_list.end(); }

    int *get_cheat_map() { return cheat_map; }

private:
    std::list<cheat_dat> cheat_list;
    int cheat_map[0x10000] { 0 };

    gb *ref_gb;
};
