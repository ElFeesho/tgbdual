project(gb_core C CXX)

set(GB_CORE_SOURCES apu.cpp cheat.cpp cpu.cpp gb.cpp gb.h lcd.cpp mbc.cpp op_cb.h op_normal.h renderer.h rom.cpp serializer.h)
add_library(gb_core ${GB_CORE_SOURCES})
target_include_directories(gb_core PUBLIC include/)
target_compile_features(gb_core PRIVATE cxx_auto_type)
