
cmake_minimum_required(VERSION 3.9)
project(Volsung)

include_directories("include")
set(CMAKE_CXX_STANDARD 17)

file(GLOB code 
    "src/*.cc"
)

if (MSVC)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /O3 /W4 /permissive-")
else()
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wfatal-errors -g -O3 -Wall -Wextra -Wpedantic")
endif()

add_library( Volsung STATIC ${code} )

set_target_properties( Volsung PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../lib )

add_executable            ( DevSandbox test/DevSandbox.cc )
target_include_directories( DevSandbox PUBLIC include )
target_link_libraries     ( DevSandbox Volsung )

add_executable            ( TestSuite test/Tests.cc )
target_include_directories( TestSuite PUBLIC include )
target_link_libraries     ( TestSuite Volsung )



add_executable            ( Volsung_Runner extra/runner/runner.cc )
target_include_directories( Volsung_Runner PUBLIC include )
target_include_directories( Volsung_Runner PUBLIC extra/runner )
target_link_libraries     ( Volsung_Runner Volsung )

if (UNIX AND NOT APPLE)
    target_link_libraries ( Volsung_Runner asound )
endif()

