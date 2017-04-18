set(SOURCES src/script_context.cpp src/script_manager.cpp)
set(INCLUDES include/osd_renderer.h include/script_context.h include/input_queue.h include/script_vm.h include/script_manager.h)
add_library(script_layer ${SOURCES} ${INCLUDES})
target_include_directories(script_layer PUBLIC include)
target_link_libraries(script_layer gb_core)

target_compile_features(script_layer PRIVATE cxx_lambdas cxx_aggregate_default_initializers)

#
# Tests
#
enable_testing()
create_test_target(script_manager src/script_manager.cpp)

target_compile_options(script_manager PRIVATE -pipe -m64 -ansi -fPIC -g -O3 -fstack-protector -fvisibility=hidden -W -Wall -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wpointer-arith -Wformat -Wreturn-type -Wsign-compare -Wmultichar -Wformat-nonliteral -Winit-self -Wuninitialized -Wno-deprecated -Wformat-security -Werror)