//
// Created by chris on 13/07/16.
//

#include <gtest/gtest.h>

#include <sstream>

#include <cheat.h>

TEST(read_altering_cheat, will_patch_rom_instantly) {
    cheat cht;
    uint16_t spy_addr = 0;
    uint8_t spy_dat = 0;
    cht.add_cheat("0001abcd", [&](uint16_t adr, uint8_t dat){
    	spy_addr = adr;
    	spy_dat = dat;
    });

    EXPECT_EQ(spy_addr, 0xcdab);
    EXPECT_EQ(spy_dat, 0x01);
}

TEST(read_altering_cheat, will_return_data_for_given_address) {
    cheat cht;
    cht.add_cheat("015babcd", [&](uint16_t, uint8_t){});
    uint8_t dat = cht.cheat_read(0, 0xcdab, 0);
    
    EXPECT_EQ(dat, 0x5b);
}

TEST(read_altering_cheat, will_return_data_adr_is_not_in_range) {
    cheat cht;
    cht.add_cheat("905BF0CF", [&](uint16_t, uint8_t){});
    cht.add_cheat("905BF0EF", [&](uint16_t, uint8_t){});
    cht.add_cheat("915BF1CF", [&](uint16_t, uint8_t){});
    cht.add_cheat("925BF2CF", [&](uint16_t, uint8_t){});
    cht.add_cheat("935BF3CF", [&](uint16_t, uint8_t){});
    cht.add_cheat("945BF4CF", [&](uint16_t, uint8_t){});
    cht.add_cheat("955BF5CF", [&](uint16_t, uint8_t){});
    cht.add_cheat("965BF6CF", [&](uint16_t, uint8_t){});
    cht.add_cheat("975BF7CF", [&](uint16_t, uint8_t){});
    
    EXPECT_EQ(cht.cheat_read(0, 0xCFF0, 0), 0x5b);
    EXPECT_EQ(cht.cheat_read(0, 0xEFF0, 0), 0x5b);
    EXPECT_EQ(cht.cheat_read(0, 0xCFF1, 0), 0x5b);
    EXPECT_EQ(cht.cheat_read(0, 0xCFF2, 0), 0x5b);
    EXPECT_EQ(cht.cheat_read(0, 0xCFF3, 0), 0x5b);
    EXPECT_EQ(cht.cheat_read(0, 0xCFF4, 0), 0x5b);
    EXPECT_EQ(cht.cheat_read(0, 0xCFF5, 0), 0x5b);
    EXPECT_EQ(cht.cheat_read(0, 0xCFF6, 0), 0x5b);
    EXPECT_EQ(cht.cheat_read(0, 0xCFF7, 0), 0x5b);
}

TEST(read_altering_cheat, will_return_default_when_ram_bank_not_correct) {
    cheat cht;
    cht.add_cheat("905BF0DF", [&](uint16_t, uint8_t){});
    
    EXPECT_EQ(cht.cheat_read(0, 0xDFF0, 0), 0x5b);
    EXPECT_EQ(cht.cheat_read(1, 0xDFF0, 0x40), 0x40);
    
}

TEST(default_value_returned, when_cheat_does_not_exist) {
    cheat cht;
    uint8_t returned_value = cht.cheat_read(0, 0xffff, 0x88);
    
    EXPECT_EQ(returned_value, 0x88);
}

TEST(default_value_returned, when_cheat_exists_but_is_not_handled) {
    cheat cht;
    cht.add_cheat("08ccffff", [&](uint16_t, uint8_t){});
    uint8_t returned_value = cht.cheat_read(0, 0xffff, 0x88);
    EXPECT_EQ(returned_value, 0x88);
}
