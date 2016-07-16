cmake_minimum_required(VERSION 3.0)

project(gb_core_test C CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CHEAT_TEST cheat.t.cpp)
set(ADD_SCAN_TEST address_scanner.t.cpp)

enable_testing()
add_subdirectory(gtest)
include_directories(${gtest_SOURCE_DIR}/include)

add_test(cheat_test cheat_test)
add_test(address_scanner_test address_scanner_test)
add_executable(cheat_test ${CHEAT_TEST})
add_executable(address_scanner_test ${ADD_SCAN_TEST})
target_link_libraries(address_scanner_test gtest gtest_main gb_core)
target_link_libraries(cheat_test gtest gtest_main gb_core)
