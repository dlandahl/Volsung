cmake_minimum_required(VERSION 3.15)
project(Yggdrasil)
include_directories("src")
set(CMAKE_CXX_STANDARD 17)

file(GLOB code 
    "src/*.h"
    "src/*.cpp"
)

add_executable(Yggdrasil ${code})
