project(gb_core C CXX)

set(GB_CORE_SOURCES apu.cpp cheat.cpp cpu.cpp gb.cpp gb.h gb_types.h lcd.cpp mbc.cpp op_cb.h op_normal.h renderer.h rom.cpp serializer.h)

add_library(gb_core ${GB_CORE_SOURCES})
