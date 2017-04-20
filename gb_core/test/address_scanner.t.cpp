#include <address_scanner.h>

#include <gtest/gtest.h>


class AddressScanner : public ::testing::Test {
public:
    AddressScanner() : memory{0, 1, 2, 3, 4, 5, 6, 7}, scanner{memory, sizeof(memory)} {}

    uint8_t memory[8];
    address_scanner scanner;
};

TEST_F(AddressScanner, can_find_an_8bit_value) {
    EXPECT_EQ(4u, scanner.find_value<uint8_t>(4)[0]);
}

TEST_F(AddressScanner, can_find_an_8bit_value_at_the_end_of_memory) {
    EXPECT_EQ(7u, scanner.find_value<uint8_t>(7)[0]);
}

TEST_F(AddressScanner, can_find_all_occurences_of_an_8bit_value) {
    memory[2] = 2;
    memory[4] = 2;
    memory[6] = 2;

    address_scan_result address = scanner.find_value<uint8_t>(2);

    EXPECT_EQ(2, address[0]);
    EXPECT_EQ(4, address[1]);
    EXPECT_EQ(6, address[2]);
}

TEST_F(AddressScanner, two_results_can_be_combined_to_find_matching_addresses) {
    address_scan_result addressOfZero = scanner.find_value<uint8_t>(0);
    address_scan_result addressOfTwo = scanner.find_value<uint8_t>(2);

    memory[0] = 2;
    memory[2] = 0;

    address_scan_result addressOfTwoWhichWasOnceZero = scanner.find_value<uint8_t>(2);

    EXPECT_EQ(0u, addressOfZero.mutual_set(addressOfTwo).size());
    EXPECT_EQ(1u, addressOfZero.mutual_set(addressOfZero).size());
    EXPECT_EQ(1u, addressOfZero.mutual_set(addressOfTwoWhichWasOnceZero).size());
}

TEST_F(AddressScanner, can_find_a_16bit_value) {
    address_scan_result address = scanner.find_value<uint16_t>(0x0001);
    EXPECT_EQ(1u, address.size());
    EXPECT_EQ(0, address[0]);
}


TEST_F(AddressScanner, can_find_a_32bit_value) {
    address_scan_result address = scanner.find_value<uint32_t>(0x00010203);
    EXPECT_EQ(1u, address.size());
    EXPECT_EQ(0, address[0]);
}

TEST_F(AddressScanner, can_find_a_32bit_value_at_the_end_of_memory) {
    address_scan_result address = scanner.find_value<uint32_t>(0x04050607);

    EXPECT_EQ(1u, address.size());
    EXPECT_EQ(4, address[0]);
}

TEST_F(AddressScanner, can_find_changed_values) {
    address_scan_state<uint8_t> result = scanner.snapshot<uint8_t>();

    memory[0] = 1;

    result = scanner.changed_value(result);

    EXPECT_EQ(1u, result.size());
}


TEST_F(AddressScanner, can_find_changed_values_twice) {
    address_scan_state<uint8_t> result = scanner.snapshot<uint8_t>();

    memory[0] = 1;
    memory[2] = 1;

    result = scanner.changed_value(result);

    EXPECT_EQ(2u, result.size());

    memory[0] = 0;

    result = scanner.changed_value(result);
    EXPECT_EQ(1u, result.size());

    result = scanner.changed_value(result);
    EXPECT_EQ(0u, result.size());
}


TEST_F(AddressScanner, can_find_increased_values) {
    address_scan_state<uint8_t> result = scanner.snapshot<uint8_t>();

    memory[0] = 1;
    memory[2] = 3;

    result = scanner.increased_value(result);

    EXPECT_EQ(2u, result.size());

    EXPECT_EQ(1, result.values().at(0));
    EXPECT_EQ(3, result.values().at(2));

    memory[0] = 0;
    memory[2] = 0;

    result = scanner.increased_value(result);

    EXPECT_EQ(0u, result.size());
}

TEST_F(AddressScanner, can_find_increased_and_changed_values) {
    address_scan_state<uint8_t> result = scanner.snapshot<uint8_t>();

    memory[0] = 1;
    memory[2] = 3;

    result = scanner.increased_value(result);

    EXPECT_EQ(2u, result.size());

    EXPECT_EQ(1, result.values().at(0));
    EXPECT_EQ(3, result.values().at(2));

    memory[0] = 0;
    memory[2] = 0;

    result = scanner.changed_value(result);

    EXPECT_EQ(2u, result.size());
}


TEST_F(AddressScanner, can_find_decreased_values) {
    address_scan_state<uint8_t> result = scanner.snapshot<uint8_t>();

    memory[1] = 0;
    memory[2] = 1;

    result = scanner.decreased_value(result);

    EXPECT_EQ(2u, result.size());

    EXPECT_EQ(0, result.values().at(1));
    EXPECT_EQ(1, result.values().at(2));
}

TEST_F(AddressScanner, can_find_unchanged) {
    address_scan_state<uint8_t> result = scanner.snapshot<uint8_t>();

    result = scanner.unchanged_value(result);

    EXPECT_EQ(8u, result.size());

    EXPECT_EQ(0, result.values().at(0));
    EXPECT_EQ(1, result.values().at(1));
}
