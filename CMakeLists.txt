
cmake_minimum_required(VERSION 3.9)
project(Volsung)

include_directories("src")
include_directories("include")
set(CMAKE_CXX_STANDARD 17)

file(GLOB code 
    "src/*.hh"
    "src/*.cc"
)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wfatal-errors -g -Wall -Wextra -Wpedantic")
add_library( Volsung STATIC
             ${code} )

set_target_properties( Volsung PROPERTIES
                       ARCHIVE_OUTPUT_DIRECTORY ../lib )

add_executable( TestProgram
                test/TestProgram.cpp )

target_include_directories( TestProgram PUBLIC
                            include )

target_link_libraries( TestProgram
                       Volsung )
                       

