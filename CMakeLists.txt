cmake_minimum_required(VERSION 3.5)

function(create_test_target target_name)
    set(OTHER_FILES "")
    foreach(f ${ARGN})
        set(OTHER_FILES ${OTHER_FILES} ${f})
    endforeach()
    add_test(${target_name} ${target_name})
    add_executable(${target_name} test/${target_name}.t.cpp ${OTHER_FILES})

    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_include_directories(${target_name} PRIVATE ${gtest_SOURCE_DIR}/include)
    target_compile_features(${target_name} PRIVATE cxx_lambdas cxx_aggregate_default_initializers)

    target_link_libraries(${target_name} gtest gtest_main)
endfunction()

add_subdirectory(gb_core)
add_subdirectory(sdl_ui)
add_subdirectory(script_layer)
add_subdirectory(gtest)

add_dependencies(tgbdual gb_core)
add_dependencies(script_layer gb_core)
add_dependencies(address_scanner_test gb_core)
add_dependencies(cheat_test gb_core)