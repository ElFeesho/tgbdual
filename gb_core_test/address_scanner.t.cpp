//
// Created by Christopher Sawczuk on 16/07/2016.
//

#include <address_scanner.h>

#include <gtest/gtest.h>
#include <array>


TEST(address_scanner, can_find_an_8bit_value)
{
	uint8_t dummy_data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	address_scanner scanner{dummy_data, sizeof(dummy_data)};

	uint8_t targetValue = 4;
	address_scan_result address = scanner.find_value(targetValue);

	EXPECT_EQ(4, address[0]);
}

TEST(address_scanner, can_find_all_occurences_of_an_8bit_value)
{
	uint8_t dummy_data[] = { 0, 1, 2, 3, 2, 5, 2, 7, 8 };
	address_scanner scanner{dummy_data, sizeof(dummy_data)};

	uint8_t targetValue = 2;
	address_scan_result address = scanner.find_value(targetValue);

	EXPECT_EQ(2, address[0]);
	EXPECT_EQ(4, address[1]);
	EXPECT_EQ(6, address[2]);
}


TEST(address_scanner, two_results_can_be_combined_to_find_matching_addresses)
{
	uint8_t dummy_data[] = { 0, 1, 2 };
	address_scanner scanner{dummy_data, sizeof(dummy_data)};

	address_scan_result addressOfZero = scanner.find_value<uint8_t>(0);

	address_scan_result addressOfTwo = scanner.find_value<uint8_t>(2);

	dummy_data[0] = 2;
	dummy_data[2] = 0;
	address_scan_result addressOfTwoWhichWasOnceZero = scanner.find_value<uint8_t>(2);

	EXPECT_EQ(0, addressOfZero.mutual_set(addressOfTwo).size());
	EXPECT_EQ(1, addressOfZero.mutual_set(addressOfZero).size());
	EXPECT_EQ(1, addressOfZero.mutual_set(addressOfTwoWhichWasOnceZero).size());
}

TEST(address_scanner, can_find_a_16bit_value)
{
	uint8_t dummy_data[] = { 0x00, 0x33, 0x66, 3, 4, 5, 6, 7, 8 };
	address_scanner scanner{dummy_data, sizeof(dummy_data)};

	uint16_t targetValue = 0x6633;
	address_scan_result address = scanner.find_value(targetValue);
	EXPECT_EQ(1, address.size());
	EXPECT_EQ(1, address[0]);
}


TEST(address_scanner, can_find_a_32bit_value)
{
	uint8_t dummy_data[] = { 0x00, 0x33, 0x66, 3, 4, 5, 6, 7, 8 };
	address_scanner scanner{dummy_data, sizeof(dummy_data)};

	uint32_t targetValue = 0x04036633;

	address_scan_result address = scanner.find_value(targetValue);
	EXPECT_EQ(1, address.size());
	EXPECT_EQ(1, address[0]);
}