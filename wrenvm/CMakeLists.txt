add_library(wrenvm src/wren_script_vm.cpp include/wren_script_vm.h)

target_link_libraries(wrenvm script_layer -L${PROJECT_SOURCE_DIR}/extlibs/wren/lib wren)

target_include_directories(wrenvm PUBLIC include/ ${PROJECT_SOURCE_DIR}/extlibs/wren/src/include)

target_compile_features(wrenvm PRIVATE cxx_lambdas cxx_aggregate_default_initializers)

target_compile_options(wrenvm PRIVATE -pipe -m64 -ansi -fPIC -g -O3 -fstack-protector -fvisibility=hidden -W -Wall -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wpointer-arith -Wformat -Wreturn-type -Wsign-compare -Wmultichar -Wformat-nonliteral -Winit-self -Wuninitialized -Wno-deprecated -Wformat-security -Werror)