set(CMAKE_CXX_FLAGS "-O3 -Wall")

set(USE_SDL ON)

set(TGBDUAL_LIBS wren z)

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

if (USE_SDL)
    list(APPEND TGBDUAL_LIBS ${SDL_LIBRARY} SDL_gfx SDL_image)
    find_package(SDL)
        message("SDL it is")
        add_definitions(-DUSE_SDL)
set(SDL_SOURCES
        src/rendering/sdl/sdl_video_renderer.cpp
        src/rendering/sdl/sdl_video_renderer.h
        src/rendering/sdl/sdl_audio_renderer.cpp
        src/rendering/sdl/sdl_audio_renderer.h
        src/input/sdl/sdl_gamepad_source.cpp
        src/input/sdl/sdl_gamepad_source.h
        src/input/sdl/sdl_console_driver.cpp
        src/input/sdl/sdl_console_driver.h
        src/emulation/sdl/sdl_core_services.cpp
        src/emulation/sdl/sdl_core_services.h)
else()
    message("SFML IT IS!")
    set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
    find_package(SFML COMPONENTS system audio graphics window)
    list(APPEND TGBDUAL_LIBS ${SFML_LIBRARIES})
    set(SFML_SOURCES
        src/emulation/sfml/sfml_core_services.cpp
        src/emulation/sfml/sfml_core_services.h
        src/rendering/sfml/sfml_video_renderer.cpp
        src/rendering/sfml/sfml_video_renderer.h
            src/input/sfml/sfml_console_driver.cpp
            src/input/sfml/sfml_console_driver.h
            src/input/sfml/sfml_gamepad_source.cpp
            src/input/sfml/sfml_gamepad_source.h
            src/input/sfml/sfml_sys_command_source.cpp
            src/input/sfml/sfml_sys_command_source.h
            src/rendering/sfml/sfml_audio_renderer.cpp
            src/rendering/sfml/sfml_audio_renderer.h)
    include_directories(${SFML_INCLUDE_DIRECTORIES})
endif()

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
        ${SDL_SOURCES}
        ${SFML_SOURCES}
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
        src/emulator_time.h
        src/rendering/video_renderer.h
        src/rendering/gb_video_renderer.cpp
        src/rendering/gb_video_renderer.h
        src/rendering/gb_osd_renderer.cpp
        src/rendering/gb_osd_renderer.h
        src/rendering/audio_renderer.h
        src/rendering/gb_audio_renderer.cpp
        src/rendering/gb_audio_renderer.h
        src/input/gb_gamepad_source.cpp
        src/input/gb_gamepad_source.h
        src/input/gamepad_source.h
        src/input/gb_sys_command_source.cpp
        src/input/gb_sys_command_source.h
        src/input/sys_command_source.h
        src/input/gb_console_driver.cpp
        src/input/gb_console_driver.h
        src/input/console_driver.h
        src/emulation/core_services.h
        src/rendering/dummy/dummy_audio_renderer.cpp
        src/rendering/dummy/dummy_audio_renderer.h
        src/rendering/dummy/dummy_video_renderer.cpp
        src/rendering/dummy/dummy_video_renderer.h
        src/input/dummy/dummy_console_driver.cpp
        src/input/dummy/dummy_console_driver.h
        src/input/dummy/dummy_gamepad_source.cpp
        src/input/dummy/dummy_gamepad_source.h
        src/input/dummy/dummy_sys_command_source.cpp
        src/input/dummy/dummy_sys_command_source.h)

find_package(LUA)

if (NOT Lua_FOUND)
    set(LUA_LIB lua5.2)
    set(LUA_INCLUDE_DIR "/usr/include/lua5.2/")
    message("Set LUA lib to ${LUA_LIB} and include directory to ${LUA_INCLUDE_DIR}")
else()
    set(LUA_LIB lua)
endif()
list(APPEND TGBDUAL_LIBS ${LUA_LIB})

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
target_link_libraries(tgbdual gb_core script_layer -L${PROJECT_SOURCE_DIR}/extlibs/wren/lib ${TGBDUAL_LIBS} -L/usr/local/lib)
target_compile_features(tgbdual PRIVATE cxx_lambdas cxx_aggregate_default_initializers)


#
# Tests
#

enable_testing()
create_test_target(console_cmd src/console/console_cmd.cpp)
create_test_target(scan_engine src/memory/scan_engine.cpp)
target_link_libraries(scan_engine gb_core)
