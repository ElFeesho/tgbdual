set(CMAKE_CXX_FLAGS "-O3 -Wall")

set(USE_SDL ON)

set(LINKCABLE_SOURCES
        src/linkcable/null_link_source.h src/linkcable/null_link_source.cpp src/linkcable/network/tcp_client.h src/linkcable/network/tcp_client.cpp
        src/linkcable/network/tcp_server.h src/linkcable/network/tcp_server.cpp src/linkcable/network/multicast_transmitter.cpp
        src/linkcable/network/multicast_transmitter.h src/linkcable/network/json.hpp src/linkcable/tcp_link_cable.cpp src/linkcable/tcp_link_cable.h
        src/linkcable/link_cable_source_provider.cpp src/linkcable/link_cable_source_provider.h)

set(TGBDUAL_DESKTOP_SOURCES
        ${LINKCABLE_SOURCES}
        src/script_commands.cpp src/script_commands.h
        src/io/buffer.h src/io/file_buffer.cpp src/io/file_buffer.h src/io/memory_buffer.cpp src/io/memory_buffer.h src/io/rom_file.cpp
        src/io/rom_file.h src/main.cpp)


set(TGBDUAL_LIBS z wrenvm luavm tgbdual)

if (USE_SDL)
    list(APPEND TGBDUAL_LIBS sdl_adapters)
    message("Using SDL adapters")
    add_definitions(-DUSE_SDL)
else()
    list(APPEND TGBDUAL_LIBS sfml_adapters)
    message("Using SFML adapters")
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_executable(tgbdual_desktop MACOSX_BUNDLE ${TGBDUAL_DESKTOP_SOURCES})
else ()
    list(APPEND TGBDUAL_LIBS pthread)
    add_executable(tgbdual_desktop ${TGBDUAL_DESKTOP_SOURCES})
endif ()


target_include_directories(tgbdual_desktop PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(tgbdual_desktop ${TGBDUAL_LIBS})

target_compile_features(tgbdual_desktop PRIVATE cxx_lambdas cxx_aggregate_default_initializers)

target_compile_options(tgbdual_desktop PRIVATE ${TGBDUAL_COMPILE_OPTIONS})

coverage_config(tgbdual_desktop)

