//
// Created by Christopher Sawczuk on 16/07/2016.
//

#pragma once


#include <cstdint>
#include <cstddef>
#include <vector>

class address_scan_result {
public:
	address_scan_result(std::vector<ptrdiff_t> locations);

	ptrdiff_t operator[](long index);

	address_scan_result mutual_set(const address_scan_result &);

	size_t size();

private:
	std::vector<ptrdiff_t> _locations;
};


class address_scanner {
public:
	address_scanner(uint8_t *memory, size_t size);

	template<typename T>
	address_scan_result find_value(T target_value) {
		std::vector<ptrdiff_t> locations;
		for (int i = 0; i < _size - sizeof(T); i++) {
			if (*((T *) (_memory + i)) == target_value) {
				locations.push_back(i);
			}
		}
		return address_scan_result{locations};
	}

private:
	uint8_t *_memory;
	size_t _size;
};
