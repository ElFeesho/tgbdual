add_library(wrenvm src/wren_script_vm.cpp include/wren_script_vm.h)

target_link_libraries(wrenvm script_layer -L${PROJECT_SOURCE_DIR}/extlibs/wren/lib wren)

target_include_directories(wrenvm PUBLIC include/ ${PROJECT_SOURCE_DIR}/extlibs/wren/src/include)

target_compile_features(wrenvm PRIVATE cxx_lambdas cxx_aggregate_default_initializers)

target_compile_options(wrenvm PRIVATE ${TGBDUAL_COMPILE_OPTIONS})