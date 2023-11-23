cmake_minimum_required(VERSION 3.10)
project(cpp_tidbits VERSION 1.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
add_executable(cpp_tidbits main.cpp 
    member_calls_ctor.cpp member_calls_ctor.hpp
    cont_string.cpp cont_string.hpp
    templates.cpp templates.hpp
    lambdas.cpp lambdas.hpp)
