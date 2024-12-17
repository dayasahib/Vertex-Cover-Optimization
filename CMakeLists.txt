cmake_minimum_required(VERSION 3.25.1)
project(PP_AE_VC)

# for profiling
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(CMAKE_CXX_STANDARD 14)

add_executable(PP_AE_VC
        src/main.cpp
        src/graph.h
        src/graph.cpp
        src/lb.h
        src/lb.cpp
        src/dr.h
        src/dr.cpp
        src/ub.h
        src/ub.cpp)
