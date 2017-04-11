#pragma once

#include <functional>
#include <address_scanner.h>
#include <vector>

class scan_engine {
public:
    using initial_scan_state_created_callback = std::function<void()>;
    using scan_callback = std::function<void(std::vector<ptrdiff_t> &)>;
    using search_callback = std::function<void(const std::map<ptrdiff_t, uint8_t> &)>;

    scan_engine(address_scanner scanner, initial_scan_state_created_callback stateCreatedCallback);

    size_t scan(uint32_t value, scan_callback cb);

    void clear_scan();

    void set_scan_threshold(size_t newThreshold);

    size_t scan_threshold();

    size_t search_unchanged(search_callback callback);

    size_t search_changed(search_callback callback);

    size_t search_increased(search_callback callback);

    size_t search_decreased(search_callback callback);

    void clear_search();

private:
    std::vector<ptrdiff_t> toVector(address_scan_result &scanResult);
    address_scanner _scanner;
    initial_scan_state_created_callback  _stateCreatedCallback;
    address_scan_result _lastResult;
    address_scan_state<uint8_t> _lastState;
    size_t _scanThreshold{3};
};
