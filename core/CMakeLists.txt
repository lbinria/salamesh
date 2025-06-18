cmake_minimum_required(VERSION 3.10)

project(salamesh_core)

set(SOURCES
    component.h component.cpp
)

# Add the library
add_library(salamesh_core ${SOURCES})