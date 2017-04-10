#pragma once

#include <functional>
#include <address_scanner.h>
#include <vector>

class scan_engine {
public:
    using scan_callback = std::function<void(std::vector<ptrdiff_t> &)>;

    scan_engine(address_scanner scanner);

    size_t scan(uint32_t value, scan_callback cb);

    void clear_scan();

    void set_scan_threshold(size_t newThreshold);

    size_t scan_threshold();

private:
    std::vector<ptrdiff_t> toVector(address_scan_result &scanResult);
    address_scanner _scanner;
    address_scan_result _lastResult;
    size_t _scanThreshold{3};
};
