set(CMAKE_CXX_FLAGS "-O3 -Wall")

set(USE_SDL ON)

set(TGBDUAL_INTERFACE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/audio_renderer.h ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering/video_renderer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/input/gamepad_source.h ${CMAKE_CURRENT_SOURCE_DIR}/src/input/console_driver.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/input/sys_command_source.h ${CMAKE_CURRENT_SOURCE_DIR}/src/emulation/core_services.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/emulator_time.h)

add_library(tgbdual_impls INTERFACE)
target_sources(tgbdual_impls INTERFACE ${TGBDUAL_INTERFACE_SOURCES})
target_include_directories(tgbdual_impls INTERFACE src/)

set(LINKCABLE_SOURCES
        src/linkcable/null_link_source.h src/linkcable/null_link_source.cpp src/linkcable/network/tcp_client.h src/linkcable/network/tcp_client.cpp
        src/linkcable/network/tcp_server.h src/linkcable/network/tcp_server.cpp src/linkcable/network/multicast_transmitter.cpp
        src/linkcable/network/multicast_transmitter.h src/linkcable/network/json.hpp src/linkcable/tcp_link_cable.cpp src/linkcable/tcp_link_cable.h
        src/linkcable/link_cable_source_provider.cpp src/linkcable/link_cable_source_provider.h)

set(CONSOLE_SOURCES
        src/console/console.cpp src/console/console.h src/console/console_cmd.cpp src/console/console_cmd.h src/commands/scan_commands.cpp
        src/commands/scan_commands.h src/commands/script_commands.cpp src/commands/script_commands.h src/commands/memory_commands.cpp
        src/commands/memory_commands.h src/commands/gameboy_commands.cpp src/commands/gameboy_commands.h)

set(TGBDUAL_DESKTOP_SOURCES
        ${LINKCABLE_SOURCES} ${SCRIPTING_SOURCES} ${CONSOLE_SOURCES} ${TGBDUAL_INTERFACE_SOURCES} src/limitter.cpp src/limitter.h
        src/io/buffer.h src/io/file_buffer.cpp src/io/file_buffer.h src/io/memory_buffer.cpp src/io/memory_buffer.h src/io/rom_file.cpp
        src/io/rom_file.h src/memory/scan_engine.cpp src/memory/scan_engine.h src/emulator_time.cpp src/emulator_time.h src/rendering/gb_video_renderer.cpp
        src/rendering/gb_video_renderer.h src/rendering/gb_osd_renderer.cpp src/rendering/gb_osd_renderer.h src/rendering/gb_audio_renderer.cpp
        src/rendering/gb_audio_renderer.h src/input/gb_gamepad_source.cpp src/input/gb_gamepad_source.h src/input/gb_sys_command_source.cpp
        src/input/gb_sys_command_source.h src/input/gb_console_driver.cpp src/input/gb_console_driver.h src/emulation/core_services.h src/main.cpp
        src/gameboy_memory_bridge.cpp src/gameboy_memory_bridge.h src/gameboy.cpp src/gameboy.h src/tgbdual.cpp src/tgbdual.h src/rom.h)


set(TGBDUAL_LIBS z wrenvm luavm)

if (USE_SDL)
    list(APPEND TGBDUAL_LIBS sdl_adapters)
    message("Using SDL adapters")
    add_definitions(-DUSE_SDL)
else()
    list(APPEND TGBDUAL_LIBS sfml_adapters)
    message("Using SFML adapters")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_executable(tgbdual MACOSX_BUNDLE ${TGBDUAL_DESKTOP_SOURCES})
else ()
    list(APPEND TGBDUAL_LIBS pthread)
    add_executable(tgbdual ${TGBDUAL_DESKTOP_SOURCES})
endif ()


target_include_directories(tgbdual PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(tgbdual gb_core script_layer ${TGBDUAL_LIBS})

target_compile_features(tgbdual PRIVATE cxx_lambdas cxx_aggregate_default_initializers)

coverage_config(tgbdual)

#
# Tests
#

enable_testing()
create_test_target(console_cmd src/console/console_cmd.cpp)
create_test_target(gb_console_driver src/input/gb_console_driver.cpp src/console/console.cpp src/console/console_cmd.cpp)
create_test_target(scan_engine src/memory/scan_engine.cpp LINK gb_core)
create_test_target(limitter src/limitter.cpp src/emulator_time.cpp)
create_test_target(emulator_time src/emulator_time.cpp)
create_test_target(gb_osd_renderer src/rendering/gb_osd_renderer.cpp src/emulator_time.cpp LINK script_layer)
create_test_target(gb_audio_renderer src/rendering/gb_audio_renderer.cpp LINK gb_core)
create_test_target(gb_video_renderer src/rendering/gb_video_renderer.cpp LINK gb_core)

target_compile_options(tgbdual PRIVATE ${TGBDUAL_COMPILE_OPTIONS})