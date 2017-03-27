#pragma once

#include <stdint.h>
#include <stdlib.h>

class gb;

class serializer;

struct rom_info {
    char cart_name[18];
    int32_t cart_type;
    uint8_t rom_size;
    uint8_t ram_size;

    bool check_sum;
    int32_t gb_type;
};

class rom {
public:
    rom();

    ~rom();

    rom_info *get_info();

    uint8_t *get_rom();

    uint8_t *get_sram();

    bool get_loaded();

    uint16_t get_sram_size();

    void set_first(int32_t page);

    bool load_rom(uint8_t *buf, size_t size, uint8_t *ram, size_t ram_size);

    void serialize(serializer &s);

private:
    rom_info info;

    uint8_t *dat;
    uint8_t *sram;

    uint8_t *first_page;

    bool b_loaded;
};