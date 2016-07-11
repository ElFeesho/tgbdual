#pragma once

#include <cstdint>

#include <list>
class gb;

struct cheat_dat {
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
