cmake_minimum_required(VERSION 3.17)
project(labcode)

set(CMAKE_CXX_STANDARD 14)

add_executable(client src/client.cpp src/tands.cpp headers/tands.h)
add_executable(server src/server.cpp src/tands.cpp headers/tands.h)