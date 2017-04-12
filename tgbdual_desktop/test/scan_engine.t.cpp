#include <gtest/gtest.h>

#include <memory/scan_engine.h>
#include <algorithm>

static inline scan_engine withMemory(uint8_t *memory, size_t memorySize, scan_engine::initial_scan_state_created_callback initialStateCallback = []{}) {
    return scan_engine{address_scanner{memory, memorySize}, initialStateCallback };
}

TEST(scan_engine, will_return_all_scan_results) {
    uint8_t memory[] = {0x00, 0x01, 0x02, 0x03};
    scan_engine engine = withMemory(memory, sizeof(memory));

    std::vector<ptrdiff_t> capturedResults;

    engine.scan(0x02, [&](std::vector<ptrdiff_t> &results) {
        std::copy(results.begin(), results.end(), std::back_inserter(capturedResults));
    });

    EXPECT_EQ(1, capturedResults.size());
    EXPECT_EQ(2, capturedResults[0]);
}

TEST(scan_engine, will_only_return_subsets) {
    uint8_t memory[] = {0x00, 0x00, 0x01, 0x00};
    scan_engine engine = withMemory(memory, sizeof(memory));

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
    scan_engine engine = withMemory(memory, sizeof(memory));

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
    scan_engine engine = withMemory(memory, sizeof(memory));

    size_t resultCount = engine.scan(0x02, [&](std::vector<ptrdiff_t> &results) {});

    EXPECT_EQ(2, resultCount);
}

TEST(scan_engine, results_not_provided_when_more_results_than_scan_threshold) {
    uint8_t memory[] = {0x02, 0x02, 0x02, 0x02};
    scan_engine engine = withMemory(memory, sizeof(memory));

    bool reported = false;
    engine.scan(0x02, [&](std::vector<ptrdiff_t> &results) {
        reported = true;
    });

    EXPECT_FALSE(reported);
}

TEST(scan_engine, scan_threshold_configurable) {
    uint8_t memory[] = {0x02, 0x02, 0x02, 0x02};
    scan_engine engine = withMemory(memory, sizeof(memory));

    engine.set_scan_threshold(4);

    bool reported = false;
    engine.scan(0x02, [&](std::vector<ptrdiff_t> &results) {
        reported = true;
    });

    EXPECT_TRUE(reported);
}

TEST(scan_engine, scan_threshold_can_be_reported) {
    uint8_t memory[] = {0x02, 0x02, 0x02, 0x02};
    scan_engine engine = withMemory(memory, sizeof(memory));

    engine.set_scan_threshold(5);
    EXPECT_EQ(5, engine.scan_threshold());
}

TEST(scan_engine, can_scan_for_16bit_values) {
    uint8_t memory[] = {0x01, 0x02, 0xff, 0xff};
    scan_engine engine = withMemory(memory, sizeof(memory));

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
    scan_engine engine = withMemory(memory, sizeof(memory));

    engine.set_scan_threshold(1);

    EXPECT_EQ(1, engine.scan(0x01ffffff, [&](std::vector<ptrdiff_t> &) {}));

    engine.clear_scan();

    EXPECT_EQ(1, engine.scan(0xffffffff, [&](std::vector<ptrdiff_t> &) {}));
}

TEST(scan_engine, will_report_initial_search_state) {
    bool initialSearchStateCreated = false;

    uint8_t memory[] = {0, 0, 0, 0, 0, 0};
    scan_engine engine = withMemory(memory, sizeof(memory), [&]{
        initialSearchStateCreated = true;
    });

    engine.search_unchanged([](const std::map<ptrdiff_t, uint8_t>&){});

    EXPECT_TRUE(initialSearchStateCreated);
}

TEST(scan_engine, initial_state_can_be_directly_created) {
    bool initialSearchStateCreated = false;

    uint8_t memory[] = {0, 0, 0, 0, 0, 0};
    scan_engine engine = withMemory(memory, sizeof(memory), [&]{
        initialSearchStateCreated = true;
    });

    engine.clear_search();

    EXPECT_TRUE(initialSearchStateCreated);
}

TEST(scan_engine, can_search_for_unchanged_values) {
    uint8_t memory[] = {0, 0, 0, 0, 0, 0};
    scan_engine engine = withMemory(memory, sizeof(memory));

    engine.clear_search();

    size_t count = engine.search_unchanged([](const std::map<ptrdiff_t, uint8_t>&){});

    EXPECT_EQ(6, count);
}

TEST(scan_engine, only_notify_of_state_created_when_required) {
    bool stateCreated = false;

    uint8_t memory[] = {0, 0, 0, 0, 0, 0};
    scan_engine engine = withMemory(memory, sizeof(memory), [&]{
        stateCreated = true;
    });

    engine.clear_search();
    stateCreated = false;

    engine.search_unchanged([](const std::map<ptrdiff_t, uint8_t>&){});

    EXPECT_FALSE(stateCreated);
}

TEST(scan_engine, will_report_unchanged_values) {
    uint8_t memory[] = {2, 2, 2, 2, 2, 2};
    scan_engine engine = withMemory(memory, sizeof(memory));

    engine.clear_search();

    engine.search_unchanged([](const std::map<ptrdiff_t, uint8_t>&){});

    memory[0] = 1;
    memory[1] = 1;
    memory[2] = 1;

    std::map<ptrdiff_t, uint8_t> capturedValues;
    engine.search_unchanged([&](const std::map<ptrdiff_t, uint8_t> &values) {
        capturedValues.insert(values.begin(), values.end());
    });

    EXPECT_EQ(2, capturedValues[0x3]);
    EXPECT_EQ(2, capturedValues[0x4]);
    EXPECT_EQ(2, capturedValues[0x5]);
}


TEST(scan_engine, will_not_report_unchanged_values_when_more_results_than_threshold) {
    uint8_t memory[] = {2, 2, 2, 2, 2, 2};
    scan_engine engine = withMemory(memory, sizeof(memory));

    engine.clear_search();

    engine.search_unchanged([](const std::map<ptrdiff_t, uint8_t>&){});

    memory[0] = 1;
    memory[1] = 1;

    bool capturedCalled = false;
    engine.search_unchanged([&](const std::map<ptrdiff_t, uint8_t> &values) {
        capturedCalled = true;
    });

    EXPECT_FALSE(capturedCalled);
}

TEST(scan_engine, will_report_changed_values) {
    uint8_t memory[] = {2, 2, 2, 2, 2, 2};
    scan_engine engine = withMemory(memory, sizeof(memory));

    engine.clear_search();

    memory[0] = 1;
    memory[1] = 1;
    memory[2] = 1;

    std::map<ptrdiff_t, uint8_t> capturedValues;
    engine.search_changed([&](const std::map<ptrdiff_t, uint8_t> &values) {
        capturedValues.insert(values.begin(), values.end());
    });

    EXPECT_EQ(1, capturedValues[0x0]);
    EXPECT_EQ(1, capturedValues[0x1]);
    EXPECT_EQ(1, capturedValues[0x2]);
}

TEST(scan_engine, will_report_decreased_values) {
    uint8_t memory[] = {2, 2, 2, 2, 2, 2};
    scan_engine engine = withMemory(memory, sizeof(memory));

    engine.clear_search();

    memory[0] = 1;
    memory[1] = 1;
    memory[2] = 1;

    std::map<ptrdiff_t, uint8_t> capturedValues;
    engine.search_decreased([&](const std::map<ptrdiff_t, uint8_t> &values) {
        capturedValues.insert(values.begin(), values.end());
    });

    EXPECT_EQ(1, capturedValues[0x0]);
    EXPECT_EQ(1, capturedValues[0x1]);
    EXPECT_EQ(1, capturedValues[0x2]);
}



TEST(scan_engine, will_report_increased_values) {
    uint8_t memory[] = {2, 2, 2, 2, 2, 2};
    scan_engine engine = withMemory(memory, sizeof(memory));

    engine.clear_search();

    memory[0] = 3;
    memory[1] = 3;
    memory[2] = 3;

    std::map<ptrdiff_t, uint8_t> capturedValues;
    engine.search_increased([&](const std::map<ptrdiff_t, uint8_t> &values) {
        capturedValues.insert(values.begin(), values.end());
    });

    EXPECT_EQ(3, capturedValues[0x0]);
    EXPECT_EQ(3, capturedValues[0x1]);
    EXPECT_EQ(3, capturedValues[0x2]);
}

