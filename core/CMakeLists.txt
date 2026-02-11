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
    script.h
    lua_script.h
    attribute.h
    element.h
    input_states.h 

    shader.cpp shader.h 

    cameras/camera.h
    cameras/trackball_camera.h
    cameras/descent_camera.h

    renderers/renderer.h
    renderers/line_renderer.cpp renderers/line_renderer.h
    renderers/point_set_renderer.cpp renderers/point_set_renderer.h
    renderers/halfedge_renderer.cpp renderers/halfedge_renderer.h
    renderers/bbox_renderer/bbox_renderer.cpp renderers/bbox_renderer/bbox_renderer.h
    renderers/clipping_renderer.cpp renderers/clipping_renderer.h
    renderers/surface_renderer.cpp renderers/surface_renderer.h
    renderers/volume_renderer.cpp renderers/volume_renderer.h
    renderers/tri_renderer.cpp renderers/tri_renderer.h
    renderers/poly_renderer.cpp renderers/poly_renderer.h
    renderers/tet_renderer.cpp renderers/tet_renderer.h
    renderers/hex_renderer.cpp renderers/hex_renderer.h

    models/model.h models/model.cpp
    models/surface_model.h
    models/volume_model.h

  # some include files to build with the lib
  ../include/stb_include.cpp 
  ${OPENGL_FILES} 
)

include_directories(../include)

# Copy renderers shaders
message("Copying PLOP from ${PROJECT_SOURCE_DIR}/renderers/bbox_renderer/plop.txt too ${CMAKE_BINARY_DIR}/shaders")
file(COPY ${PROJECT_SOURCE_DIR}/renderers/bbox_renderer/bbox.frag DESTINATION ${CMAKE_BINARY_DIR}/shaders)
file(COPY ${PROJECT_SOURCE_DIR}/renderers/bbox_renderer/bbox.vert DESTINATION ${CMAKE_BINARY_DIR}/shaders)

# Add the library
add_library(salamesh_core ${SOURCES} ${JSON_SOURCES})