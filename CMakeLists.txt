
cmake_minimum_required(VERSION 3.9)
project(Volsung)

include_directories("src")
include_directories("include")
set(CMAKE_CXX_STANDARD 17)

file(GLOB code 
    "src/*.hh"
    "src/*.cc"
)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wfatal-errors -g -O3 -Wall -Wextra -Wpedantic")
add_library( Volsung STATIC
             ${code} )

set_target_properties( Volsung PROPERTIES
                       ARCHIVE_OUTPUT_DIRECTORY ../lib )

add_executable( DevSandbox
                test/DevSandbox.cc )
add_executable( TestSuite
                test/Tests.cc )

target_include_directories( DevSandbox PUBLIC
                            include )
target_include_directories( TestSuite PUBLIC
                            include )

target_link_libraries( DevSandbox
                       Volsung )
target_link_libraries( TestSuite
                       Volsung )
                       

