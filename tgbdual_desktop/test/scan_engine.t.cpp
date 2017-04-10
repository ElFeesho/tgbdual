#include <gtest/gtest.h>

#include <algorithm>
#include <vector>
#include <functional>
#include <scan_engine.h>


TEST(scan_engine, will_return_all_scan_results) {
    uint8_t memory[] = {0x00, 0x01, 0x02, 0x03};
    address_scanner scanner{memory, 4};
    scan_engine engine{scanner};

    std::vector<ptrdiff_t> capturedResults;

    engine.scan(0x02, [&](std::vector<ptrdiff_t> &results) {
        std::copy(results.begin(), results.end(), std::back_inserter(capturedResults));
    });

    EXPECT_EQ(1, capturedResults.size());
    EXPECT_EQ(2, capturedResults[0]);
}

TEST(scan_engine, will_only_return_subsets) {
    uint8_t memory[] = {0x00, 0x00, 0x01, 0x00};
    address_scanner scanner{memory, 4};
    scan_engine engine{scanner};

    std::vector<ptrdiff_t> capturedResults;

    engine.scan(0x01, [&](std::vector<ptrdiff_t> &results) {

    });

    memory[2] = 0x00;

    engine.scan(0x00, [&](std::vector<ptrdiff_t> &results) {
        std::copy(results.begin(), results.end(), std::back_inserter(capturedResults));
    });

    EXPECT_EQ(1, capturedResults.size());
    EXPECT_EQ(2, capturedResults[0]);
}

TEST(scan_engine, scan_state_can_be_reset) {
    uint8_t memory[] = {0x00, 0x02, 0x01, 0x02};
    address_scanner scanner{memory, 4};
    scan_engine engine{scanner};

    std::vector<ptrdiff_t> capturedResults;

    engine.scan(0x01, [&](std::vector<ptrdiff_t> &results) {
        std::copy(results.begin(), results.end(), std::back_inserter(capturedResults));
    });

    EXPECT_EQ(1, capturedResults.size());
    EXPECT_EQ(2, capturedResults[0]);
    capturedResults.clear();

    memory[2] = 0x02;

    engine.clear_scan();

    engine.scan(0x02, [&](std::vector<ptrdiff_t> &results) {
        std::copy(results.begin(), results.end(), std::back_inserter(capturedResults));
    });

    EXPECT_EQ(3, capturedResults.size());
    EXPECT_EQ(1, capturedResults[0]);
    EXPECT_EQ(2, capturedResults[1]);
}


TEST(scan_engine, number_of_matches_returned) {
    uint8_t memory[] = {0x00, 0x02, 0x01, 0x02};
    address_scanner scanner{memory, 4};
    scan_engine engine{scanner};

    size_t resultCount = engine.scan(0x02, [&](std::vector<ptrdiff_t> &results) {});

    EXPECT_EQ(2, resultCount);
}

TEST(scan_engine, results_not_provided_when_more_results_than_scan_threshold) {
    uint8_t memory[] = {0x02, 0x02, 0x02, 0x02};
    address_scanner scanner{memory, 4};
    scan_engine engine{scanner};

    bool reported = false;
    engine.scan(0x02, [&](std::vector<ptrdiff_t> &results) {
        reported = true;
    });

    EXPECT_FALSE(reported);
}

TEST(scan_engine, scan_threshold_configurable) {
    uint8_t memory[] = {0x02, 0x02, 0x02, 0x02};
    address_scanner scanner{memory, 4};
    scan_engine engine{scanner};
    engine.set_scan_threshold(4);

    bool reported = false;
    engine.scan(0x02, [&](std::vector<ptrdiff_t> &results) {
        reported = true;
    });

    EXPECT_TRUE(reported);
}

TEST(scan_engine, scan_threshold_can_be_reported) {
    uint8_t memory[] = {0x02, 0x02, 0x02, 0x02};
    address_scanner scanner{memory, 4};
    scan_engine engine{scanner};
    engine.set_scan_threshold(5);
    EXPECT_EQ(5, engine.scan_threshold());
}

TEST(scan_engine, can_scan_for_16bit_values) {
    uint8_t memory[] = {0x01, 0x02, 0xff, 0xff};

    address_scanner scanner{memory, 4};
    scan_engine engine{scanner};

    engine.set_scan_threshold(1);

    size_t results = engine.scan(0x0102, [&](std::vector<ptrdiff_t> &) {});
    EXPECT_EQ(1, results);

    engine.clear_scan();

    results = engine.scan(0x02ff, [&](std::vector<ptrdiff_t> &) {});
    EXPECT_EQ(1, results);

    engine.clear_scan();

    results = engine.scan(0xffff, [&](std::vector<ptrdiff_t> &) {});
    EXPECT_EQ(1, results);
}

TEST(scan_engine, can_scan_for_32bit_values) {
    uint8_t memory[] = {0x01, 0xff, 0xff, 0xff, 0xff};

    address_scanner scanner{memory, 5};
    scan_engine engine{scanner};

    engine.set_scan_threshold(1);

    EXPECT_EQ(1, engine.scan(0x01ffffff, [&](std::vector<ptrdiff_t> &) {}));

    engine.clear_scan();

    EXPECT_EQ(1, engine.scan(0xffffffff, [&](std::vector<ptrdiff_t> &) {}));
}
