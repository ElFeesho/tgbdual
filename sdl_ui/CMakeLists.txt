cmake_minimum_required(VERSION 3.0)

project(tgbdual C CXX)

set(CMAKE_CXX_FLAGS "-O3 -Wall")

set(SDL_UI_SOURCES tcp_client.h tcp_client.cpp link_cable_source.h tcp_server.h tcp_server.cpp multicast_transmitter.cpp multicast_transmitter.h cpputil.h dialogs.h dinput_sdl.c dinput_sdl.h resource.cpp resource.h sdl_renderer.cpp sdl_renderer.h sdlmain.cpp setting.cpp setting.h unzip.c unzip.h w32_posix.h zlibwrap.cpp zlibwrap.h json.hpp)

find_package(SDL)

include_directories(${SDL_INCLUDE_DIR})

add_executable(tgbdual ${SDL_UI_SOURCES})

target_link_libraries(tgbdual gb_core goomba ${SDL_LIBRARY} z)
target_compile_features(tgbdual PRIVATE cxx_lambdas)
