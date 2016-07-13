//
// Created by chris on 13/07/16.
//

#include <gtest/gtest.h>

#include <sstream>

#include <cheat.h>

TEST(cheat_can_be_added, when_valid) {
    auto cheatDat = new cheat_dat("01020304");

    EXPECT_EQ(cheatDat->code, 1);
    EXPECT_EQ(cheatDat->dat, 2);
    EXPECT_EQ(cheatDat->adr, 0x0403);
}
