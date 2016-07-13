//
// Created by chris on 13/07/16.
//

#include <gtest/gtest.h>

#include <cheat.h>

TEST(cheat_can_be_added, when_valid) {
    auto cheatDat = new cheat_dat("test name", "01020304");

    ASSERT_EQ(cheatDat->code, 0x01);
    ASSERT_EQ(cheatDat->dat, 0x02);
    ASSERT_EQ(cheatDat->adr, 0x0403);
}

