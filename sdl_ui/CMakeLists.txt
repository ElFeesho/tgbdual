cmake_minimum_required(VERSION 3.0)

project(tgbdual C CXX)

set(CMAKE_CXX_FLAGS "-O3 -Wall")

set(SDL_UI_SOURCES 
	null_link_source.h 
	null_link_source.cpp 
	tcp_client.h 
	tcp_client.cpp 
	link_cable_source.h 
	tcp_server.h 
	tcp_server.cpp 
	multicast_transmitter.cpp 
	multicast_transmitter.h 
	sdl_renderer.cpp 
	sdl_renderer.h 
	sdlmain.cpp 
	json.hpp)

find_package(SDL)

include_directories(${SDL_INCLUDE_DIR})

add_executable(tgbdual ${SDL_UI_SOURCES})

target_link_libraries(tgbdual gb_core ${SDL_LIBRARY} z)
target_compile_features(tgbdual PRIVATE cxx_lambdas)
