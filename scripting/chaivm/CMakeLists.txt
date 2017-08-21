add_subdirectory(chai_ext)

add_library(chaivm src/chai_script_vm.cpp include/chai_script_vm.h)
target_link_libraries(chaivm script_layer ${CHAISCRIPT_LIBS})
target_include_directories(chaivm PUBLIC chai_ext/include)

target_include_directories(chaivm PUBLIC include/)
target_compile_features(chaivm PRIVATE cxx_lambdas cxx_aggregate_default_initializers)

target_compile_options(chaivm PRIVATE ${TGBDUAL_COMPILE_OPTIONS})

coverage_config(chaivm)

create_test_target(chai_script_vm LINK chaivm)
