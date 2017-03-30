//
// Created by Christopher Sawczuk on 16/07/2016.
//

#include "address_scanner.h"

#include <algorithm>

address_scanner::address_scanner(uint8_t *memory, size_t size) : _memory{memory}, _size{size} {

}

template<typename T>
address_scan_result address_scanner::find_value(T target_value) {
	std::vector<ptrdiff_t> locations;
	for (int i = 0; i < _size - sizeof(T); i++) {
		if (*((T *) (_memory + i)) == target_value) {
			locations.push_back(i);
		}
	}
	return address_scan_result{locations};
}

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
