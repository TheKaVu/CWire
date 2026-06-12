cmake_minimum_required(VERSION 4.2)
project(CWire)

set(CMAKE_CXX_STANDARD 20)

add_library(CWire SHARED
        src/library.cpp
        src/delegate.hpp
        src/multidelegate.hpp
        src/event.hpp
        src/property.hpp
)
target_include_directories(CWire PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties(CWire PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
