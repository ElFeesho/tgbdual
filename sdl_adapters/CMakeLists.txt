find_package(SDL)

add_library(sdl_adapters
        src/emulation/sdl_core_services.cpp
        src/emulation/sdl_core_services.h
        src/rendering/sdl_audio_renderer.cpp
        src/rendering/sdl_audio_renderer.h
        src/rendering/sdl_video_renderer.cpp
        src/rendering/sdl_video_renderer.h
        src/input/sdl_console_driver.cpp
        src/input/sdl_console_driver.h
        src/input/sdl_gamepad_source.cpp
        src/input/sdl_gamepad_source.h)

target_link_libraries(sdl_adapters PUBLIC ${SDL_LIBRARY} SDL_gfx SDL_image tgbdual_impls)
target_include_directories(sdl_adapters PRIVATE ${SDL_INCLUDE_DIR} src/)
target_compile_features(sdl_adapters PRIVATE cxx_lambdas cxx_aggregate_default_initializers)
