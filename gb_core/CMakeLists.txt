cmake_minimum_required(VERSION 3.0)

project(gb_core C CXX)

set(GB_CORE_SOURCES address_scanner.cpp gameboy.cpp gameboy.h link_cable_source.h apu.cpp gamepad_source.h cheat.cpp cpu.cpp gb.cpp gb.h lcd.cpp mbc.cpp op_cb.h op_normal.h renderer.h rom.cpp serializer.h address_scanner.cpp address_scanner.h)
add_library(gb_core ${GB_CORE_SOURCES})
target_include_directories(gb_core PUBLIC include/)
target_compile_features(gb_core PRIVATE cxx_auto_type)
