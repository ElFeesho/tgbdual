set(GB_CORE_SOURCES
        include/sound_provider.h
        include/link_cable_source.h
        include/gamepad_source.h
        include/gb.h
        include/video_renderer.h
        include/audio_renderer.h
        include/serializer.h
        include/address_scanner.h
        include/apu.h
        include/cheat.h
        include/cpu.h
        include/lcd.h
        include/mbc.h
        include/rom.h
        include/gbtime.h
        src/address_scanner.cpp
        src/apu.cpp
        src/cheat.cpp
        src/cpu.cpp
        src/gb.cpp
        src/lcd.cpp
        src/mbc.cpp
        src/rom.cpp
        src/gbtime.cpp)

add_library(gb_core STATIC ${GB_CORE_SOURCES})
target_include_directories(gb_core PUBLIC include/)
target_compile_features(gb_core PRIVATE cxx_auto_type)
target_compile_options(gb_core PRIVATE ${TGBDUAL_COMPILE_OPTIONS})

coverage_config(gb_core)

#
# Tests
#
enable_testing()

create_test_target(cheat LINK gb_core)
create_test_target(address_scanner LINK gb_core)