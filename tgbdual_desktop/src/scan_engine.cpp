#include "scan_engine.h"

scan_engine::scan_engine(address_scanner scanner) : _scanner{scanner}, _lastResult{{}} {
}

size_t scan_engine::scan(uint32_t value, scan_engine::scan_callback cb) {

    address_scan_result result;

    if (value < 256) {
        result = _scanner.find_value<uint8_t>(value);
    }
    else if(value < (1<<16)){
        result = _scanner.find_value<uint16_t>(value);
    }
    else {
        result = _scanner.find_value<uint32_t>(value);
    }

    if (_lastResult.size() > 0) {
        result = result.mutual_set(_lastResult);
    }

    _lastResult = result;

    if (_lastResult.size() <= _scanThreshold) {
        std::vector<ptrdiff_t> results = toVector(_lastResult);

        cb(results);
    }

    return _lastResult.size();
}

void scan_engine::clear_scan() {
    _lastResult = address_scan_result{{}};
}

std::vector<ptrdiff_t> scan_engine::toVector(address_scan_result &scanResult) {
    std::vector<ptrdiff_t> results{};
    for (int i = 0; i < scanResult.size(); i++) {
        results.push_back(scanResult[i]);
    }
    return results;
}

void scan_engine::set_scan_threshold(size_t newThreshold) {
    _scanThreshold = newThreshold;
}

size_t scan_engine::scan_threshold() {
    return _scanThreshold;
}
