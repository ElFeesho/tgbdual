set(CMAKE_CXX_FLAGS "-O3 -Wall")

set(SDL_UI_SOURCES
        src/network/null_link_source.h src/network/null_link_source.cpp
        src/network/tcp_client.h src/network/tcp_client.cpp
        src/network/tcp_server.h src/network/tcp_server.cpp
        src/network/multicast_transmitter.cpp src/network/multicast_transmitter.h
        src/sdl_renderer.cpp src/sdl_renderer.h
        src/limitter.cpp src/limitter.h
        src/input/sdl_gamepad_source.cpp src/input/sdl_gamepad_source.h
        src/io/file_buffer.cpp src/io/file_buffer.h
        src/io/memory_buffer.cpp src/io/memory_buffer.h
        src/input/joystick_input_source.cpp src/input/joystick_input_source.h
        src/input/keyboard_input_source.cpp src/input/keyboard_input_source.h
        src/io/buffer.h
        src/sdlmain.cpp
        src/input/input_source.h
        src/json.hpp
        src/RomFile.cpp
        src/RomFile.h
        src/console/Console.cpp src/console/Console.h
        src/scripting/lua_script_vm.cpp src/scripting/lua_script_vm.h
        src/scripting/wren_script_vm.cpp src/scripting/wren_script_vm.h
        src/console/ConsoleCmd.cpp src/console/ConsoleCmd.h
        src/link_cable_source_provider.cpp src/link_cable_source_provider.h)

find_package(SDL)
find_package(LUA)
set(LUA_LIB "lua")

if (!Lua_FOUND)
    set(LUA_LIB lua5.2)
    set(LUA_INCLUDE_DIR "/usr/include/lua5.2/")
    message("Set LUA lib to ${LUA_LIB} and include directory to ${LUA_INCLUDE_DIR}")
endif()

include_directories(${SDL_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/extlibs/wren/src/include)

add_executable(tgbdual ${SDL_UI_SOURCES})
target_include_directories(tgbdual PRIVATE ${LUA_INCLUDE_DIR})

target_include_directories(tgbdual PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(tgbdual PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(tgbdual gb_core script_layer ${SDL_LIBRARY} -L${PROJECT_SOURCE_DIR}/extlibs/wren/lib ${LUA_LIB} wren z -L/usr/local/lib SDL_gfx SDL_image)
target_compile_features(tgbdual PRIVATE cxx_lambdas cxx_aggregate_default_initializers)


#
# Tests
#

enable_testing()
create_test_target(console_cmd src/console/ConsoleCmd.cpp)
