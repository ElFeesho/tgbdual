#include <gtest/gtest.h>
#include <limitter.h>
#include <emulator_time.h>

TEST(Limitter, will_sleep_16ms_whenOperationTakes0ms) {

    uint32_t sleepDuration = INT32_MAX;

    emulator_time::set_sleep_provider([&](uint32_t timeToSleep) {
        sleepDuration = timeToSleep;
    });

    emulator_time::set_time_provider([]{ return 0; });

    limitter rate_limitter{[]{}};

    rate_limitter.limit();

    EXPECT_EQ(16u, sleepDuration);
}

TEST(Limitter, will_sleep_1ms_whenOperationTakes0ms_in_fast_mode) {

    uint32_t sleepDuration = INT32_MAX;

    emulator_time::set_sleep_provider([&](uint32_t timeToSleep) {
        sleepDuration = timeToSleep;
    });

    emulator_time::set_time_provider([]{ return 0; });

    limitter rate_limitter{[]{}};

    rate_limitter.fast();

    rate_limitter.limit();

    EXPECT_EQ(1u, sleepDuration);
}

TEST(Limitter, will_sleep_8ms_when_operation_takes_8ms) {

    uint32_t sleepDuration = INT32_MAX;

    emulator_time::set_sleep_provider([&](uint32_t timeToSleep) {
        sleepDuration = timeToSleep;
    });

    emulator_time::set_time_provider([]{ return 0; });

    limitter rate_limitter{[]{
        emulator_time::set_time_provider([]{ return 8; });
    }};

    rate_limitter.limit();

    EXPECT_EQ(8u, sleepDuration);
}

TEST(Limitter, will_not_sleep_when_operation_takes_longer_than_16ms) {
    bool didntSleep = true;

    emulator_time::set_sleep_provider([&](uint32_t timeToSleep) {
        didntSleep = false;
    });

    emulator_time::set_time_provider([]{ return 0; });

    limitter rate_limitter{[]{
        emulator_time::set_time_provider([]{ return 17; });
    }};

    rate_limitter.limit();

    EXPECT_TRUE(didntSleep);
}

TEST(Limitter, will_not_sleep_when_operation_takes_longer_than_1ms_in_fast_mode) {
    bool didntSleep = true;

    emulator_time::set_sleep_provider([&](uint32_t timeToSleep) {
        didntSleep = false;
    });

    emulator_time::set_time_provider([]{ return 0; });

    limitter rate_limitter{[]{
        emulator_time::set_time_provider([]{ return 1; });
    }};
    rate_limitter.fast();
    rate_limitter.limit();

    EXPECT_TRUE(didntSleep);
}
