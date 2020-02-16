
cmake_minimum_required(VERSION 3.9)
project(Volsung)

include_directories("include")
set(CMAKE_CXX_STANDARD 17)

file(GLOB code 
    "src/*.cc"
)

if (MSVC)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4 /permissive- /std:c++17")
else()
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wfatal-errors -g -O3 -Wall -Wextra -Wpedantic")
endif()

add_library( Volsung STATIC ${code} )
set_target_properties( Volsung PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ../lib )

add_executable            ( DevSandbox test/DevSandbox.cc )
target_include_directories( DevSandbox PUBLIC include )
target_link_libraries     ( DevSandbox Volsung )

add_executable            ( Test_Parser test/Tests.cc )
target_include_directories( Test_Parser PUBLIC include )
target_link_libraries     ( Test_Parser Volsung )



add_executable            ( volsung_player extra/Player/VlsngPlayer.cc )
target_include_directories( volsung_player PUBLIC include )
target_include_directories( volsung_player PUBLIC extra/Player )
target_link_libraries     ( volsung_player Volsung )

if (UNIX AND NOT APPLE)
    target_link_libraries ( volsung_player asound )
elseif (APPLE)
    target_link_libraries ( volsung_player "-framework AudioToolbox" )
    target_link_libraries ( volsung_player "-framework CoreFoundation" )
    target_sources ( volsung_player PRIVATE extra/Player/AudioPlayer_Macos.cc )
elseif (WIN32)
    target_link_libraries ( volsung_player dsound)
endif()

