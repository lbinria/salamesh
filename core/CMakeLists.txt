cmake_minimum_required(VERSION 3.12...3.26)

project(salamesh_core)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    app_interface.h
    hex_model_interface.h
    component.h component.cpp
    attribute.h
    element.h
    states.h 
    model.h
    camera.h
)

# Add the library
add_library(salamesh_core ${SOURCES})