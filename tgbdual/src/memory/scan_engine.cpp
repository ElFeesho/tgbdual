#include "scan_engine.h"

scan_engine::scan_engine(address_scanner scanner, initial_scan_state_created_callback stateCreatedCallback) :
        _scanner{scanner},
        _stateCreatedCallback{stateCreatedCallback},
        _lastResult{{}} {
}

size_t scan_engine::scan(uint32_t value, scan_engine::scan_callback cb) {

    address_scan_result result;

    if (value < 256) {
        result = _scanner.find_value((uint8_t) value);
    }
    else if(value < (1<<16)){
        result = _scanner.find_value((uint16_t) value);
    }
    else {
        result = _scanner.find_value(value);
    }

    if (_lastResult.size() > 0) {
        result = result.mutual_set(_lastResult);
    }

    _lastResult = result;

    if (_lastResult.size() > 0 && _lastResult.size() <= _scanThreshold) {
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
    for (size_t i = 0; i < scanResult.size(); i++) {
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

size_t scan_engine::search_unchanged(search_callback callback) {
    if (_lastState.size() == 0) {
        _stateCreatedCallback();
    }
    _lastState = _scanner.unchanged_value(_lastState);

    if (_lastState.size() <= _scanThreshold){
        callback(_lastState.values());
    }

    return _lastState.size();
}

size_t scan_engine::search_changed(scan_engine::search_callback callback) {
    if (_lastState.size() == 0) {
        _stateCreatedCallback();
    }
    _lastState = _scanner.changed_value(_lastState);

    if (_lastState.size() <= _scanThreshold){
        callback(_lastState.values());
    }

    return _lastState.size();
}

size_t scan_engine::search_increased(scan_engine::search_callback callback) {
    if (_lastState.size() == 0) {
        _stateCreatedCallback();
    }
    _lastState = _scanner.increased_value(_lastState);

    if (_lastState.size() <= _scanThreshold){
        callback(_lastState.values());
    }

    return _lastState.size();
}

size_t scan_engine::search_decreased(scan_engine::search_callback callback) {
    if (_lastState.size() == 0) {
        _stateCreatedCallback();
    }
    _lastState = _scanner.decreased_value(_lastState);

    if (_lastState.size() <= _scanThreshold){
        callback(_lastState.values());
    }

    return _lastState.size();
}

void scan_engine::clear_search() {
    _lastState = _scanner.snapshot<uint8_t>();
    _stateCreatedCallback();
}
