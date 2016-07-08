#pragma once

#include <stdint.h>

class serializer;
class gb;

class mbc {
   public:
    mbc(gb *ref);

    uint8_t *get_rom() { return rom_page; }
    uint8_t *get_sram() { return sram_page; }
    bool is_ext_ram() { return ext_is_ram; }
    void set_ext_is(bool ext) { ext_is_ram = ext; }

    int get_state();
    void set_state(int dat);
    void set_page(int rom, int sram);

    uint8_t read(uint16_t adr);
    void write(uint16_t adr, uint8_t dat);
    uint8_t ext_read(uint16_t adr);
    void ext_write(uint16_t adr, uint8_t dat);
    void reset();

    void serialize(serializer &s);

   private:
    void mbc1_write(uint16_t adr, uint8_t dat);
    void mbc2_write(uint16_t adr, uint8_t dat);
    void mbc3_write(uint16_t adr, uint8_t dat);
    void mbc5_write(uint16_t adr, uint8_t dat);
    void mbc7_write(uint16_t adr, uint8_t dat);
    void huc1_write(uint16_t adr, uint8_t dat);
    void huc3_write(uint16_t adr, uint8_t dat);
    void tama5_write(uint16_t adr, uint8_t dat);
    void mmm01_write(uint16_t adr, uint8_t dat);

    uint8_t *rom_page;
    uint8_t *sram_page;

    bool mbc1_16_8;
    uint8_t mbc1_dat;

    uint8_t mbc3_latch; // 1 bits
    uint8_t mbc3_sec;   // 6
    uint8_t mbc3_min;   // 6
    uint8_t mbc3_hour;  // 5
    uint8_t mbc3_dayl;  // 8
    uint8_t mbc3_dayh;  // 1

    uint8_t mbc3_timer; // 4
    bool ext_is_ram;    // 1
    // total 32bits

    int mbc5_dat;

    bool mbc7_write_enable;
    bool mbc7_idle;
    uint8_t mbc7_cs;
    uint8_t mbc7_sk;
    uint8_t mbc7_op_code;
    uint8_t mbc7_adr;
    uint16_t mbc7_dat;
    uint8_t mbc7_ret;
    uint8_t mbc7_state;
    uint16_t mbc7_buf;
    uint8_t mbc7_count;

    bool huc1_16_8;
    uint8_t huc1_dat;

    gb *ref_gb;
};