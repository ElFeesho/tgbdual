#pragma once

#include <stdint.h>
#include <stdlib.h>

class gb;

class serializer;

struct rom_info {
    char cart_name[18] { 0 };
    int32_t cart_type { 0 };
    uint8_t rom_size { 0 };
    uint8_t ram_size { 0 };

    int32_t gb_type { 0 };
};

class rom {
public:
    ~rom();

    rom_info *get_info();

    uint8_t *get_rom();

    uint8_t *get_sram();

    uint16_t get_sram_size();

    void set_first(int32_t page);

    bool load_rom(uint8_t *buf, size_t size, uint8_t *ram, size_t ram_size);

    void serialize(serializer &s);

private:
    rom_info info;

    uint8_t *dat { nullptr };
    uint8_t *sram { nullptr };

    uint8_t *first_page { nullptr };

    bool b_loaded { false };
};