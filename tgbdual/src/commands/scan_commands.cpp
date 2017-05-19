#include "scan_commands.h"

#include <string>
#include <sstream>
#include <memory/scan_engine.h>

static inline std::string to_string(int value) {
    std::stringstream s;
    s << value;
    return s.str();
}

static inline void printAddressValue(tgbdual &tgb, uint32_t address, uint8_t value) {
    std::stringstream s;
    s << "0x" << std::hex << address << ": " << std::dec << (uint32_t) value << " (0x" << std::hex << (uint32_t) value << std::dec << ")";
    tgb.addConsoleOutput(s.str());
}

void print_scan_state(tgbdual &tgb, std::string searchType, const std::map<ptrdiff_t, uint8_t> &valuesMap) {
    tgb.addConsoleOutput(searchType);
    for (auto &values : valuesMap) {
        printAddressValue(tgb, (uint32_t) values.first, values.second);
    }
}


void printScanResults(std::vector<ptrdiff_t> &results, tgbdual &tgb) {
    int i = 0;
    for (ptrdiff_t &value : results) {
        std::stringstream s;
        s << std::dec << ++i << ": 0x" << std::hex << value;
        tgb.addConsoleOutput(s.str());
    }
}

void registerScanCommands(tgbdual &tgb) {
    static scan_engine scanEngine{tgb.createAddressScanner(), std::bind(&tgbdual::addConsoleOutput, &tgb, "Initial search state created")};
    tgb.addConsoleCommand("scan", [&](std::vector<std::string> args) {

        if (args.size() != 1) {
            tgb.addConsoleErrorOutput("Usage: scan [value]");
            return;
        }

        uint32_t value = console_cmd::toInt<uint32_t>(args[0]);
        size_t resultCount{0};
        resultCount = scanEngine.scan(value, [&](std::vector<ptrdiff_t> &results) {
            printScanResults(results, tgb);
        });
        tgb.addConsoleOutput("Found " + to_string((int) resultCount) + " results");
    });

    tgb.addConsoleCommand("start_search", [&](std::vector<std::string> args) {
        scanEngine.clear_search();
    });

    tgb.addConsoleCommand("search_greater", [&](std::vector<std::string> args) {
        size_t resultCount = scanEngine.search_increased([&](const std::map<ptrdiff_t, uint8_t> &results) {
            print_scan_state(tgb, "Greater values", results);
        });
        tgb.addConsoleOutput("Total values: " + to_string(resultCount));
    });

    tgb.addConsoleCommand("search_lesser", [&](std::vector<std::string> args) {
        size_t resultCount = scanEngine.search_decreased([&](const std::map<ptrdiff_t, uint8_t> &results) {
            print_scan_state(tgb, "Lesser values", results);
        });
        tgb.addConsoleOutput("Total values: " + to_string(resultCount));
    });

    tgb.addConsoleCommand("search_changed", [&](std::vector<std::string> args) {
        size_t resultCount = scanEngine.search_changed([&](const std::map<ptrdiff_t, uint8_t> &results) {
            print_scan_state(tgb, "Changed values", results);
        });
        tgb.addConsoleOutput("Total values: " + to_string(resultCount));
    });

    tgb.addConsoleCommand("search_unchanged", [&](std::vector<std::string> args) {
        size_t resultCount = scanEngine.search_unchanged([&](const std::map<ptrdiff_t, uint8_t> &results) {
            print_scan_state(tgb, "Unchanged values", results);
        });
        tgb.addConsoleOutput("Total values: " + to_string(resultCount));
    });

    tgb.addConsoleCommand("scan_threshold", [&](std::vector<std::string> args) {
        if (args.size() == 0) {
            tgb.addConsoleOutput("Scan threshold: " + to_string(scanEngine.scan_threshold()));
        } else {
            scanEngine.set_scan_threshold(console_cmd::toInt<size_t>(args[0]));
            tgb.addConsoleOutput("Scan threshold now: " + to_string(scanEngine.scan_threshold()));
        }
    });

    tgb.addConsoleCommand("clear_scan", [&](std::vector<std::string> args) {
        scanEngine.clear_scan();
        tgb.addConsoleOutput("Cleared previous search results");
    });
}

