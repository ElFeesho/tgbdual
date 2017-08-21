cmake_minimum_required(VERSION 3.2)
project(tgbdual C CXX)

SET(CMAKE_CXX_FLAGS "-O0")
SET(CMAKE_C_FLAGS "-O0")

option(ENABLE_CODE_COVERAGE "Enable code coverage" OFF)

function(coverage_config target_name)
    if (ENABLE_CODE_COVERAGE)
        set_target_properties(${target_name} PROPERTIES COMPILE_FLAGS "-O0 -ftest-coverage -fprofile-arcs" LINK_FLAGS "--coverage")
    endif ()
endfunction()

function(create_test_target target_name)
    set(OTHER_FILES "")
    set(OTHER_LIBS "")

    set(INLIBS OFF)
    foreach (f ${ARGN})
        if (${f} STREQUAL LINK)
            set(INLIBS ON)
        else ()
            if (${INLIBS})
                set(OTHER_LIBS ${OTHER_LIBS} ${f})
            else ()
                set(OTHER_FILES ${OTHER_FILES} ${f})
            endif ()
        endif ()
    endforeach ()
    add_test(${target_name} ${target_name})
    add_executable(${target_name} test/${target_name}.t.cpp ${OTHER_FILES})
    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_include_directories(${target_name} PRIVATE ${PROJECT_SOURCE_DIR}/gtest/include)
    target_compile_features(${target_name} PRIVATE cxx_lambdas cxx_aggregate_default_initializers)
    target_compile_options(${target_name} PRIVATE ${TGBDUAL_COMPILE_OPTIONS})
    target_link_libraries(${target_name} ${OTHER_LIBS} gtest gtest_main)

    coverage_config(${target_name})
endfunction()

if (ENABLE_CODE_COVERAGE)
    set(TGBDUAL_COMPILE_OPTIONS -pipe -ansi -fPIC -g -O0 -fstack-protector -fvisibility=hidden
            -W -Wall -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wpointer-arith
            -Wformat -Wreturn-type -Wsign-compare -Wmultichar -Wformat-nonliteral -Winit-self
            -Wuninitialized -Wno-deprecated -Wformat-security -Werror)
else ()
    set(TGBDUAL_COMPILE_OPTIONS -pipe -ansi -fPIC -g -O3 -fstack-protector -fvisibility=hidden
            -W -Wall -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wpointer-arith
            -Wformat -Wreturn-type -Wsign-compare -Wmultichar -Wformat-nonliteral -Winit-self
            -Wuninitialized -Wno-deprecated -Wformat-security -Werror)
endif ()

enable_testing()

add_subdirectory(gb_core)
add_subdirectory(tgbdual)
add_subdirectory(tgbdual_desktop)
add_subdirectory(sdl_adapters)
add_subdirectory(sfml_adapters)
add_subdirectory(dummy_adapters)
add_subdirectory(script_layer)
add_subdirectory(scripting)
add_subdirectory(gtest)

add_custom_target(coverage-report
        COMMAND lcov --directory ${CMAKE_CURRENT_BINARY_DIR} --capture --output-file coverage.info
        COMMAND lcov --remove coverage.info '*/test/*' '*gtest*' '*/include/*' '*.h' '/usr/*' '*v1*' --output-file coverage-clean.info
        COMMAND genhtml coverage-clean.info -o report
        COMMAND lcov --directory ${CMAKE_CURRENT_BINARY_DIR} --zerocounters)
