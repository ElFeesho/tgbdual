add_library(dummy_adapters
        src/rendering/dummy_audio_renderer.cpp
        src/rendering/dummy_audio_renderer.h
        src/rendering/dummy_video_renderer.cpp
        src/rendering/dummy_video_renderer.h
        src/input/dummy_console_driver.cpp
        src/input/dummy_console_driver.h
        src/input/dummy_gamepad_source.cpp
        src/input/dummy_gamepad_source.h
        src/input/dummy_sys_command_source.cpp
        src/input/dummy_sys_command_source.h)

target_link_libraries(dummy_adapters PUBLIC tgbdual_impls)
target_include_directories(dummy_adapters PUBLIC src/)
target_compile_features(dummy_adapters PRIVATE cxx_lambdas cxx_aggregate_default_initializers)
