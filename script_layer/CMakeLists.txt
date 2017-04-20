set(SOURCES src/script_context.cpp src/script_manager.cpp)
set(INCLUDES include/osd_renderer.h include/script_context.h include/input_queue.h include/script_vm.h include/script_manager.h include/memory_bridge.h)
add_library(script_layer ${SOURCES} ${INCLUDES})
target_include_directories(script_layer PUBLIC include)

target_compile_features(script_layer PRIVATE cxx_lambdas cxx_aggregate_default_initializers)

#
# Tests
#
create_test_target(script_manager src/script_manager.cpp)
create_test_target(script_context LINK script_layer)