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
    component.h
    attribute.h
    element.h
    input_states.h 
    model.h
    camera.h

    shader.cpp shader.h 

    renderers/renderer.h
    renderers/line_renderer.cpp renderers/line_renderer.h
    renderers/point_set_renderer.cpp renderers/point_set_renderer.h
    renderers/halfedge_renderer.cpp renderers/halfedge_renderer.h
    renderers/bbox_renderer.cpp renderers/bbox_renderer.h
    renderers/clipping_renderer.cpp renderers/clipping_renderer.h
    renderers/surface_renderer.cpp renderers/surface_renderer.h
    renderers/volume_renderer.cpp renderers/volume_renderer.h
    renderers/tri_renderer.cpp renderers/tri_renderer.h
    renderers/quad_renderer.cpp renderers/quad_renderer.h
    renderers/poly_renderer.cpp renderers/poly_renderer.h
    renderers/tet_renderer.cpp renderers/tet_renderer.h
    renderers/hex_renderer.cpp renderers/hex_renderer.h

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