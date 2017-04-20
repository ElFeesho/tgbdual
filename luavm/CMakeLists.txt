find_package(LUA)

if (NOT Lua_FOUND)
    set(LUA_LIB lua5.2)
    set(LUA_INCLUDE_DIR "/usr/include/lua5.2/")
    message("Set LUA lib to ${LUA_LIB} and include directory to ${LUA_INCLUDE_DIR}")
else()
    set(LUA_LIB lua)
endif()

add_library(luavm src/lua_script_vm.cpp include/lua_script_vm.h)
target_link_libraries(luavm script_layer ${LUA_LIB})
target_include_directories(luavm PUBLIC ${LUA_INCLUDE_DIR})
target_include_directories(luavm PUBLIC include/)
target_compile_features(luavm PRIVATE cxx_lambdas cxx_aggregate_default_initializers)

target_compile_options(luavm PRIVATE ${TGBDUAL_COMPILE_OPTIONS})

create_test_target(lua_script_vm LINK luavm)
