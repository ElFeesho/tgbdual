//
// Created by Christopher Sawczuk on 16/07/2016.
//

#include <gtest/gtest.h>
#include <array>

class address_scan_result
{
public:
	address_scan_result(std::vector<ptrdiff_t> locations);
	ptrdiff_t operator[](long index);
	address_scan_result mutual_set(const address_scan_result &);
	size_t size();
private:
	std::vector<ptrdiff_t> _locations;
};

address_scan_result::address_scan_result(std::vector<ptrdiff_t> locations) : _locations{locations} {}

ptrdiff_t address_scan_result::operator[](long index) {
	return _locations[index];
}

address_scan_result address_scan_result::mutual_set(const address_scan_result &target) {
	std::vector<ptrdiff_t> intersection;
	std::set_intersection(_locations.begin(), _locations.end(), target._locations.begin(), target._locations.end(), std::back_inserter(intersection));
	return address_scan_result{intersection};
}

size_t address_scan_result::size() {
	return _locations.size();
}

class address_scanner
{
public:
	address_scanner(uint8_t *memory, size_t size);

	template<typename T>
	address_scan_result find_value(T target_value);

private:
	uint8_t *_memory;
	size_t _size;
};

address_scanner::address_scanner(uint8_t *memory, size_t size) : _memory{memory}, _size{size} {

}

template<typename T>
address_scan_result address_scanner::find_value(T target_value) {
	std::vector<ptrdiff_t> locations;
	for (int i = 0; i < _size-sizeof(T); i++)
	{
		if (*((T*)(_memory+i)) == target_value)
		{
			locations.push_back(i);
		}
	}
	return address_scan_result{locations};
}


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