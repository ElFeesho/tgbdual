set(SOURCES src/script_context.cpp)
set(INCLUDES include/osd_renderer.h include/script_context.h include/input_queue.h include/macro_runner.h)
add_library(script_layer ${SOURCES} ${INCLUDES})
target_include_directories(script_layer PUBLIC include)
target_link_libraries(script_layer gb_core)

target_compile_features(script_layer PRIVATE cxx_lambdas cxx_aggregate_default_initializers)