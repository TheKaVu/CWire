cmake_minimum_required(VERSION 4.2)
project(CWire)

set(CMAKE_CXX_STANDARD 20)

add_library(CWire SHARED library.cpp
        delegate.hpp
        multidelegate.hpp
        event.hpp
        property.hpp
)
