cmake_minimum_required(VERSION 3.5...4.1.2)

project(salamesh_core)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(JSON_SOURCES
    ../include/json.hpp
)

if(WIN32)
  set(OPENGL_FILES)
else()
  set(OPENGL_FILES ../include/glad.c)
endif(WIN32)

set(SOURCES
    app_interface.h
    component.h component.cpp
    attribute.h
    element.h
    input_states.h 
    renderer.h
    model.h
    camera.h

    shader.cpp shader.h 
    line_renderer.cpp line_renderer.h
    point_set_renderer.cpp point_set_renderer.h
    halfedge_renderer.cpp halfedge_renderer.h
    bbox_renderer.cpp bbox_renderer.h
    clipping_renderer.cpp clipping_renderer.h
    surface_renderer.cpp surface_renderer.h
    volume_renderer.cpp volume_renderer.h
    tri_renderer.cpp tri_renderer.h
    quad_renderer.cpp quad_renderer.h
    poly_renderer.cpp poly_renderer.h
    tet_renderer.cpp tet_renderer.h
    hex_renderer.cpp hex_renderer.h
    tri_model.cpp tri_model.h
    quad_model.cpp quad_model.h
    poly_model.cpp poly_model.h
    tet_model.cpp tet_model.h
    hex_model.cpp hex_model.h

  # some include files to build with the lib
  ../include/stb_include.cpp 
  ${OPENGL_FILES} 
)

include_directories(../include)


# Add the library
add_library(salamesh_core ${SOURCES} ${JSON_SOURCES})