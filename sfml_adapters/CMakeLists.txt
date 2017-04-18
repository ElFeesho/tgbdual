set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(SFML COMPONENTS system audio graphics window)

add_library(sfml_adapters
        src/emulation/sfml_core_services.cpp
        src/emulation/sfml_core_services.h
        src/rendering/sfml_audio_renderer.cpp
        src/rendering/sfml_audio_renderer.h
        src/rendering/sfml_video_renderer.cpp
        src/rendering/sfml_video_renderer.h
        src/input/sfml_console_driver.cpp
        src/input/sfml_console_driver.h
        src/input/sfml_gamepad_source.cpp
        src/input/sfml_gamepad_source.h
        src/input/sfml_sys_command_source.cpp
        src/input/sfml_sys_command_source.h)

target_link_libraries(sfml_adapters PUBLIC ${SFML_LIBRARIES} tgbdual_impls dummy_adapters)
target_include_directories(sfml_adapters PRIVATE ${SFML_INCLUDE_DIRECTORIES} src/)
target_compile_features(sfml_adapters PRIVATE cxx_lambdas cxx_aggregate_default_initializers)

target_compile_options(sfml_adapters PRIVATE -pipe -m64 -ansi -fPIC -g -O3 -fno-exceptions -fstack-protector -fvisibility=hidden -W -Wall -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wpointer-arith -Wformat -Wreturn-type -Wsign-compare -Wmultichar -Wformat-nonliteral -Winit-self -Wuninitialized -Wno-deprecated -Wformat-security -Werror)
