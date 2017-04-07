cmake_minimum_required(VERSION 3.5)

add_subdirectory(gb_core)
add_subdirectory(sdl_ui)
add_subdirectory(gb_core_test)
add_subdirectory(script_layer)

add_dependencies(tgbdual gb_core)
add_dependencies(script_layer gb_core)
add_dependencies(address_scanner_test gb_core)
add_dependencies(cheat_test gb_core)