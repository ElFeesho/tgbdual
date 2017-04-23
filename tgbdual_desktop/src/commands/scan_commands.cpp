#include "scan_commands.h"

#include <sstream>

static inline void printAddressValue(console &console, uint32_t address, uint8_t value) {
    std::stringstream s;
    s << "0x" << std::hex << address << ": " << std::dec << (uint32_t) value << " (0x" << std::hex << (uint32_t) value << std::dec << ")";
    console.addOutput(s.str());
}

void print_scan_state(console &console, std::string searchType, const std::map<ptrdiff_t, uint8_t> &valuesMap) {
    console.addOutput(searchType);
    for (auto &values : valuesMap) {
        printAddressValue(console, (uint32_t) values.first, values.second);
    }
}


void printScanResults(std::vector<ptrdiff_t> &results, console &console) {
    int i = 0;
    for (ptrdiff_t &value : results) {
        std::stringstream s;
        s << std::hex << value;
        console.addOutput(std::to_string(++i) + ": 0x" + s.str());
    }
}

void registerScanCommands(console &console, scan_engine &scanEngine) {
    console.addCommand("scan", [&](std::vector<std::string> args) {

        if (args.size() != 1) {
            console.addError("Usage: scan [value]");
            return;
        }

        uint32_t value = console_cmd::toInt<uint32_t>(args[0]);
        size_t resultCount{0};
        resultCount = scanEngine.scan(value, [&](std::vector<ptrdiff_t> &results) {
            printScanResults(results, console);
        });
        console.addOutput("Found " + std::to_string((int) resultCount) + " results");
    });

    console.addCommand("start_search", [&](std::vector<std::string> args) {
        scanEngine.clear_search();
    });

    console.addCommand("search_greater", [&](std::vector<std::string> args) {
        size_t resultCount = scanEngine.search_increased([&](const std::map<ptrdiff_t, uint8_t> &results) {
            print_scan_state(console, "Greater values", results);
        });
        console.addOutput("Total values: " + std::to_string(resultCount));
    });

    console.addCommand("search_lesser", [&](std::vector<std::string> args) {
        size_t resultCount = scanEngine.search_decreased([&](const std::map<ptrdiff_t, uint8_t> &results) {
            print_scan_state(console, "Lesser values", results);
        });
        console.addOutput("Total values: " + std::to_string(resultCount));
    });

    console.addCommand("search_changed", [&](std::vector<std::string> args) {
        size_t resultCount = scanEngine.search_changed([&](const std::map<ptrdiff_t, uint8_t> &results) {
            print_scan_state(console, "Changed values", results);
        });
        console.addOutput("Total values: " + std::to_string(resultCount));
    });

    console.addCommand("search_unchanged", [&](std::vector<std::string> args) {
        size_t resultCount = scanEngine.search_unchanged([&](const std::map<ptrdiff_t, uint8_t> &results) {
            print_scan_state(console, "Unchanged values", results);
        });
        console.addOutput("Total values: " + std::to_string(resultCount));
    });

    console.addCommand("scan_threshold", [&](std::vector<std::string> args) {
        if (args.size() == 0) {
            console.addOutput("Scan threshold: " + std::to_string(scanEngine.scan_threshold()));
        } else {
            scanEngine.set_scan_threshold(console_cmd::toInt<size_t>(args[0]));
            console.addOutput("Scan threshold now: " + std::to_string(scanEngine.scan_threshold()));
        }
    });

    console.addCommand("clear_scan", [&](std::vector<std::string> args) {
        scanEngine.clear_scan();
        console.addOutput("Cleared previous search results");
    });
}

