set(CMAKE_CXX_FLAGS "-O3 -Wall")

set(LINKCABLE_SOURCES
        src/linkcable/null_link_source.h
        src/linkcable/null_link_source.cpp
        src/linkcable/network/tcp_client.h
        src/linkcable/network/tcp_client.cpp
        src/linkcable/network/tcp_server.h
        src/linkcable/network/tcp_server.cpp
        src/linkcable/network/multicast_transmitter.cpp
        src/linkcable/network/multicast_transmitter.h
        src/linkcable/network/json.hpp
        src/linkcable/tcp_link_cable.cpp
        src/linkcable/tcp_link_cable.h
        src/linkcable/link_cable_source_provider.cpp
        src/linkcable/link_cable_source_provider.h)

set(SDL_RENDERING_SOURCES
        src/rendering/sdl/sdl_video_renderer.cpp
        src/rendering/sdl/sdl_video_renderer.h
        src/rendering/sdl/sdl_audio_renderer.cpp
        src/rendering/sdl/sdl_audio_renderer.h)

set(SDL_INPUT_SOURCES
        src/input/sdl/sdl_gamepad_source.cpp
        src/input/sdl/sdl_gamepad_source.h
        src/input/sdl/joystick_input_source.cpp
        src/input/sdl/joystick_input_source.h
        src/input/sdl/keyboard_input_source.cpp
        src/input/sdl/keyboard_input_source.h)

set(SCRIPTING_SOURCES
        src/scripting/lua_script_vm.cpp
        src/scripting/lua_script_vm.h
        src/scripting/wren_script_vm.cpp
        src/scripting/wren_script_vm.h)

set(CONSOLE_SOURCES
        src/console/console.cpp
        src/console/console.h
        src/console/console_cmd.cpp
        src/console/console_cmd.h
        src/commands/scan_commands.cpp
        src/commands/scan_commands.h
        src/commands/script_commands.cpp
        src/commands/script_commands.h
        src/commands/memory_commands.cpp
        src/commands/memory_commands.h
        src/commands/gameboy_commands.cpp
        src/commands/gameboy_commands.h)

set(TGBDUAL_DESKTOP_SOURCES
        ${LINKCABLE_SOURCES}
        ${SDL_RENDERING_SOURCES}
        ${SDL_INPUT_SOURCES}
        ${SCRIPTING_SOURCES}
        ${CONSOLE_SOURCES}
        src/limitter.cpp src/limitter.h
        src/io/buffer.h
        src/io/file_buffer.cpp src/io/file_buffer.h
        src/io/memory_buffer.cpp src/io/memory_buffer.h
        src/io/rom_file.cpp src/io/rom_file.h
        src/memory/scan_engine.cpp src/memory/scan_engine.h
        src/main.cpp
        src/emulator_time.cpp
        src/emulator_time.h src/rendering/video_renderer.h src/rendering/gb_video_renderer.cpp src/rendering/gb_video_renderer.h src/rendering/gb_osd_renderer.cpp src/rendering/gb_osd_renderer.h src/rendering/audio_renderer.h src/rendering/gb_audio_renderer.cpp src/rendering/gb_audio_renderer.h)

find_package(SDL)
find_package(LUA)
set(LUA_LIB "lua")

if (NOT Lua_FOUND)
    set(LUA_LIB lua5.2)
    set(LUA_INCLUDE_DIR "/usr/include/lua5.2/")
    message("Set LUA lib to ${LUA_LIB} and include directory to ${LUA_INCLUDE_DIR}")
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_executable(tgbdual MACOSX_BUNDLE ${TGBDUAL_DESKTOP_SOURCES})
else ()
    add_executable(tgbdual ${TGBDUAL_DESKTOP_SOURCES})
endif ()

target_include_directories(tgbdual PRIVATE ${SDL_INCLUDE_DIR})
target_include_directories(tgbdual PRIVATE ${PROJECT_SOURCE_DIR}/extlibs/wren/src/include)

target_include_directories(tgbdual PRIVATE ${LUA_INCLUDE_DIR})

target_include_directories(tgbdual PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(tgbdual PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(tgbdual gb_core script_layer ${SDL_LIBRARY} -L${PROJECT_SOURCE_DIR}/extlibs/wren/lib ${LUA_LIB} wren z -L/usr/local/lib SDL_gfx SDL_image)
target_compile_features(tgbdual PRIVATE cxx_lambdas cxx_aggregate_default_initializers)


#
# Tests
#

enable_testing()
create_test_target(console_cmd src/console/console_cmd.cpp)
create_test_target(scan_engine src/memory/scan_engine.cpp)
target_link_libraries(scan_engine gb_core)
