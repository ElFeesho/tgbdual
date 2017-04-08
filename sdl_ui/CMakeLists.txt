cmake_minimum_required(VERSION 3.0)

project(tgbdual C CXX)

set(CMAKE_CXX_FLAGS "-O3 -Wall")

set(SDL_UI_SOURCES
        src/network/null_link_source.h
        src/network/null_link_source.cpp
        src/network/tcp_client.h
        src/network/tcp_client.cpp
        src/network/tcp_server.h
        src/network/tcp_server.cpp
        src/network/multicast_transmitter.cpp
        src/network/multicast_transmitter.h
        src/sdl_renderer.cpp
        src/sdl_renderer.h
        src/limitter.cpp
        src/limitter.h
        src/input/sdl_gamepad_source.cpp
        src/input/sdl_gamepad_source.h
        src/io/file_buffer.cpp
        src/io/file_buffer.h
        src/io/memory_buffer.cpp
        src/io/memory_buffer.h
        src/input/joystick_input_source.cpp
        src/input/joystick_input_source.h
        src/input/keyboard_input_source.cpp
        src/input/keyboard_input_source.h
        src/io/buffer.h
        src/sdlmain.cpp
        src/input/input_source.h
        src/json.hpp
        src/RomFile.cpp
        src/RomFile.h
        src/console/Console.cpp
        src/console/Console.h
        src/scripting/lua_macro_runner.cpp
        src/scripting/lua_macro_runner.h
        src/scripting/wren_macro_runner.cpp
        src/scripting/wren_macro_runner.h src/console/ConsoleCmd.cpp src/console/ConsoleCmd.h src/link_cable_source_provider.cpp src/link_cable_source_provider.h)

find_package(SDL)

include_directories(${SDL_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../extlibs/lua-5.3.3/include)
include_directories(${PROJECT_SOURCE_DIR}/../extlibs/wren/src/include)
message(${PROJECT_SOURCE_DIR}/../extlibs/lua-5.3.3/include)

add_executable(tgbdual ${SDL_UI_SOURCES})
target_include_directories(tgbdual PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(tgbdual gb_core script_layer ${SDL_LIBRARY} -L${PROJECT_SOURCE_DIR}/../extlibs/wren/lib -L${PROJECT_SOURCE_DIR}/../extlibs/lua-5.3.3/bin wren z lua -L/usr/local/lib SDL_gfx SDL_image)
target_compile_features(tgbdual PRIVATE cxx_lambdas cxx_aggregate_default_initializers)


#
# Tests
#
function(create_test_target target_name)
    set(OTHER_FILES "")
    foreach(f ${ARGN})
        set(OTHER_FILES ${OTHER_FILES} ${f})
    endforeach()
    add_test(${target_name} ${target_name})
    add_executable(${target_name} test/${target_name}.t.cpp ${OTHER_FILES})

    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_compile_features(${target_name} PRIVATE cxx_lambdas cxx_aggregate_default_initializers)

    target_link_libraries(${target_name} gtest gtest_main)
endfunction()

enable_testing()
include_directories(${gtest_SOURCE_DIR}/include)

create_test_target(script_manager)
create_test_target(console_cmd src/console/ConsoleCmd.cpp)
