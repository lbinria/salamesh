cmake_minimum_required(VERSION 3.10)

project(salamesh_core)

set(SOURCES
    app_interface.h
    component.h component.cpp
    element.h
    states.h 
    model.h
    camera.h
)

# Add the library
add_library(salamesh_core ${SOURCES})