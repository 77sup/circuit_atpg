cmake_minimum_required(VERSION 3.0)
project(circuit_atpg)
#set(CMAKE_CXX_FLAGS_RELEASE “-O2”)
#绝对路径写法：include_directories(${CMAKE_SOURCE_DIR/include}),#CMAKE_SOURCE_DIR等价于根目录
include_directories(${CMAKE_SOURCE_DIR}/include)
set(CMAKE_BUILD_TYPE "DEBUG")
add_definitions("-g")
add_executable(c_atpg src/main.cpp src/iscas89_parser.cpp src/circuit_graph.cpp src/log.cpp )
