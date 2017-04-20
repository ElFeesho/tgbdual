#include <gtest/gtest.h>
#include <emulator_time.h>

TEST(emulator_time, will_use_provided_time_provider) {
    emulator_time::set_time_provider([]{ return 1234u; });
    EXPECT_EQ(1234u, emulator_time::current_time());
}
