cmake_minimum_required(VERSION 3.0)

project(tgbdual C CXX)

set(CMAKE_CXX_FLAGS "-O3 -Wall")

set(SDL_UI_SOURCES 
	null_link_source.h 
	null_link_source.cpp 
	tcp_client.h 
	tcp_client.cpp 
	tcp_server.h 
	tcp_server.cpp 
	multicast_transmitter.cpp 
	multicast_transmitter.h 
	sdl_renderer.cpp 
	sdl_renderer.h 
	sdl_gamepad_source.cpp
	sdl_gamepad_source.h
	file_buffer.cpp
	file_buffer.h
	memory_buffer.cpp
	memory_buffer.h
	joystick_input_source.cpp
	joystick_input_source.h
	keyboard_input_source.cpp
	keyboard_input_source.h
	buffer.h
	sdlmain.cpp 
	json.hpp macro_runner.cpp macro_runner.h osd_renderer.h script_context.cpp script_context.h)

find_package(SDL)

include_directories(${SDL_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../extlibs/lua-5.3.3/include)
message(${PROJECT_SOURCE_DIR}/../extlibs/lua-5.3.3/include)

add_executable(tgbdual ${SDL_UI_SOURCES})

target_link_libraries(tgbdual gb_core ${SDL_LIBRARY} z -L${PROJECT_SOURCE_DIR}/../extlibs/lua-5.3.3/bin lua -L/usr/local/lib SDL_gfx)
target_compile_features(tgbdual PRIVATE cxx_lambdas)
