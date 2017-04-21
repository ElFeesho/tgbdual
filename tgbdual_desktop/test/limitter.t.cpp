#include <gtest/gtest.h>
#include <limitter.h>
#include <emulator_time.h>

class LimitterTest : public ::testing::Test {
public:
protected:
    void SetUp() override {
        emulator_time::set_sleep_provider([&](uint32_t timeToSleep) {
            sleptFor = timeToSleep;
            slept = true;
        });
        emulator_time::set_time_provider([&]{ return timeToProvide; });
    }

    bool slept{false};
    uint32_t sleptFor{INT32_MAX};
    uint32_t timeToProvide{0};
    uint32_t nextTimeToProvide{0};

    limitter rate_limitter{[&]{
        timeToProvide = nextTimeToProvide;
    }};
};

TEST_F(LimitterTest, will_sleep_16ms_whenOperationTakes0ms) {

    rate_limitter.limit();

    EXPECT_EQ(16u, sleptFor);
}

TEST_F(LimitterTest, will_sleep_1ms_whenOperationTakes0ms_in_fast_mode) {
    rate_limitter.fast();

    rate_limitter.limit();

    EXPECT_EQ(1u, sleptFor);
}

TEST_F(LimitterTest, will_sleep_8ms_when_operation_takes_8ms) {

    nextTimeToProvide = 8u;

    rate_limitter.limit();

    EXPECT_EQ(8u, sleptFor);
}

TEST_F(LimitterTest, will_not_sleep_when_operation_takes_longer_than_16ms) {
    nextTimeToProvide = 17u;

    rate_limitter.limit();

    EXPECT_FALSE(slept);
}

TEST_F(LimitterTest, will_not_sleep_when_operation_takes_longer_than_1ms_in_fast_mode) {
    nextTimeToProvide = 1;

    rate_limitter.fast();
    rate_limitter.limit();

    EXPECT_FALSE(slept);
}

TEST_F(LimitterTest, will_return_to_normal_speed) {
    nextTimeToProvide = 1;

    rate_limitter.fast();
    rate_limitter.limit();

    EXPECT_FALSE(slept);

    rate_limitter.normal();
    slept = false;

    rate_limitter.limit();

    EXPECT_TRUE(slept);
}
