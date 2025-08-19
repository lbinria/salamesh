cmake_minimum_required(VERSION 3.12...3.26)

project(salamesh_core)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    app_interface.h
    component.h component.cpp
    attribute.h
    element.h
    states.h 
    renderer.h
    model.h
    tet_model_interface.h
    hex_model_interface.h
    tri_model_interface.h
    quad_model_interface.h
    camera.h
)

# Add the library
add_library(salamesh_core ${SOURCES})