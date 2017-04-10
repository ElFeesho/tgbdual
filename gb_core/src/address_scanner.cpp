//
// Created by Christopher Sawczuk on 16/07/2016.
//

#include "address_scanner.h"

address_scanner::address_scanner(uint8_t *memory, size_t size) : _memory{memory}, _size{size} {

}


address_scan_result::address_scan_result() : _locations{} {

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

