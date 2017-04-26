set(TGBDUAL_INTERFACE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/audio_renderer.h ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/video_renderer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/input/gamepad_source.h ${CMAKE_CURRENT_SOURCE_DIR}/src/input/console_driver.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/input/sys_command_source.h ${CMAKE_CURRENT_SOURCE_DIR}/src/emulation/core_services.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/emulator_time.h)

add_library(tgbdual_impls INTERFACE)
target_sources(tgbdual_impls INTERFACE ${TGBDUAL_INTERFACE_SOURCES})
target_include_directories(tgbdual_impls INTERFACE src/)

set(SOURCES src/emulator_time.cpp src/emulator_time.h
        src/commands/gameboy_commands.cpp src/commands/gameboy_commands.h
        src/commands/memory_commands.cpp src/commands/memory_commands.h
        src/commands/scan_commands.cpp src/commands/scan_commands.h
        src/console/console.cpp src/console/console.h src/console/console_cmd.cpp src/console/console_cmd.h
        src/emulation/core_services.h
        src/memory/scan_engine.cpp src/memory/scan_engine.h
        src/rendering/audio_renderer.h src/rendering/gb_audio_renderer.cpp src/rendering/gb_audio_renderer.h
        src/rendering/gb_osd_renderer.cpp src/rendering/gb_osd_renderer.h src/rendering/gb_video_renderer.cpp src/rendering/gb_video_renderer.h
        src/rendering/video_renderer.h
        src/gameboy.h src/gameboy.cpp
        src/gameboy_memory_bridge.cpp src/gameboy_memory_bridge.h
        src/rom.h
        src/limitter.cpp src/limitter.h
        src/tgbdual.cpp src/tgbdual.h
        src/input/console_driver.h src/input/gamepad_source.h src/input/gb_console_driver.cpp src/input/gb_console_driver.h
        src/input/gb_gamepad_source.cpp src/input/gb_sys_command_source.h src/input/gb_sys_command_source.cpp)

add_library(tgbdual ${SOURCES})
target_link_libraries(tgbdual PUBLIC gb_core script_layer)
target_include_directories(tgbdual PUBLIC src/)
target_compile_features(tgbdual PRIVATE cxx_lambdas cxx_aggregate_default_initializers)
target_compile_options(tgbdual PRIVATE ${TGBDUAL_COMPILE_OPTIONS})

#
# Tests
#

create_test_target(console_cmd src/console/console_cmd.cpp)
create_test_target(gb_console_driver src/input/gb_console_driver.cpp src/console/console.cpp src/console/console_cmd.cpp)
create_test_target(scan_engine src/memory/scan_engine.cpp LINK gb_core)
create_test_target(limitter src/limitter.cpp src/emulator_time.cpp)
create_test_target(emulator_time src/emulator_time.cpp)
create_test_target(gb_osd_renderer src/rendering/gb_osd_renderer.cpp src/emulator_time.cpp LINK script_layer)
create_test_target(gb_audio_renderer src/rendering/gb_audio_renderer.cpp LINK gb_core)
create_test_target(gb_video_renderer src/rendering/gb_video_renderer.cpp LINK gb_core)