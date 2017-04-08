cmake_minimum_required(VERSION 3.0)

project(gb_core C CXX)

set(GB_CORE_SOURCES
        include/sound_renderer.h
        include/gameboy.h
        include/link_cable_source.h
        include/gamepad_source.h
        include/gb.h
        include/renderer.h
        include/serializer.h
        include/address_scanner.h
        include/apu.h
        include/cheat.h
        include/cpu.h
        include/lcd.h
        include/mbc.h
        include/rom.h
        src/address_scanner.cpp
        src/gameboy.cpp
        src/apu.cpp
        src/cheat.cpp
        src/cpu.cpp
        src/gb.cpp
        src/lcd.cpp
        src/mbc.cpp
        src/rom.cpp)

add_library(gb_core STATIC ${GB_CORE_SOURCES})
target_include_directories(gb_core PUBLIC include/)
target_compile_features(gb_core PRIVATE cxx_auto_type)

#
# Tests
#
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include)

add_test(cheat_test cheat_test)
add_test(address_scanner_test address_scanner_test)
add_test(console_cmd_test console_cmd_test)

add_executable(cheat_test test/cheat.t.cpp)
add_executable(address_scanner_test test/address_scanner.t.cpp)

target_link_libraries(address_scanner_test gtest gtest_main gb_core)
target_link_libraries(cheat_test gtest gtest_main gb_core)
