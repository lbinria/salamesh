cmake_minimum_required(VERSION 3.10)

project(salamesh_core)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(IMGUI_SOURCES
#     ../include/imgui/imgui.cpp
#     ../include/imgui/imgui.h
    
#     ../include/imgui/imgui_demo.cpp
#     ../include/imgui/imgui_draw.cpp
#     ../include/imgui/imgui_widgets.cpp
#     ../include/imgui/imgui_tables.cpp

#     ../include/imgui/backends/imgui_impl_glfw.h
#     ../include/imgui/backends/imgui_impl_glfw.cpp
#     ../include/imgui/backends/imgui_impl_opengl3.h
#     ../include/imgui/backends/imgui_impl_opengl3.cpp
#     ../include/imgui/backends/imgui_impl_opengl3_loader.h
# )

# # Create an interface library for ImGui
# add_library(salamesh_imgui INTERFACE)

# target_include_directories(salamesh_imgui INTERFACE
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/imgui
# )

# # Tell consumers what sources to compile
# target_sources(salamesh_imgui INTERFACE
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/imgui/imgui.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/imgui/imgui_demo.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/imgui/imgui_draw.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/imgui/imgui_widgets.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/imgui/imgui_tables.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/imgui/backends/imgui_impl_glfw.cpp
#   ${CMAKE_CURRENT_SOURCE_DIR}/../include/imgui/backends/imgui_impl_opengl3.cpp
# )

# # Public dependencies that ImGui needs
# target_link_libraries(salamesh_imgui INTERFACE
#   glfw
#   OpenGL::GL
# )


# LUA
IF(WIN32)
  # Set the path to the Lua library and include directory
  set(LUA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/lua-5.4.2_Win64_dll17_lib")
  set(LUA_INCLUDE_DIR "${LUA_DIR}/include")
  set(LUA_LIB_DIR "${LUA_DIR}")

  # Include the lib directory
  include_directories(${LUA_INCLUDE_DIR})
  # Directly link the Lua library
  set(LUA_LIBRARIES "${LUA_LIB_DIR}/lua54.lib") # Change this to the actual library filename

  # Ensure that the library was found
  if(NOT LUA_LIBRARIES)
      message(FATAL_ERROR "Could not find Lua library")
  endif()


  # Create an imported library target for Lua
  add_library(lua_lib UNKNOWN IMPORTED)
  set_target_properties(lua_lib PROPERTIES
      IMPORTED_LOCATION "${LUA_DIR}/lua54.lib"
      INTERFACE_INCLUDE_DIRECTORIES "${LUA_DIR}/include"
  )
ELSE()
  find_package(Lua REQUIRED)
ENDIF(WIN32)


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
    scene_interface.h
    scene_view_interface.h
    script.h
    lua_script.h
    module.h
    attribute.h
    element.h
    input_states.h 
    navigation_path.h 

    shader.cpp shader.h 

    render_surface.cpp render_surface.h

    instanciator.h
    cameras/camera_collection.h
    renderers/renderer_collection.h
    models/model_collection.h


    cameras/camera.h
    cameras/trackball_camera.h
    cameras/descent_camera.h

    renderers/renderer_info.h
    renderers/renderer.h
    renderers/mesh_renderer.h
    renderers/line_renderer.cpp renderers/line_renderer.h
    renderers/point_set_renderer.cpp renderers/point_set_renderer.h
    renderers/halfedge_renderer.cpp renderers/halfedge_renderer.h
    renderers/bbox_renderer/bbox_renderer.cpp renderers/bbox_renderer/bbox_renderer.h
    renderers/clipping_renderer.cpp renderers/clipping_renderer.h
    renderers/surface_renderer.cpp renderers/surface_renderer.h
    renderers/surface_renderer2.cpp renderers/surface_renderer2.h
    renderers/volume_renderer.cpp renderers/volume_renderer.h
    renderers/tri_renderer.cpp renderers/tri_renderer.h
    renderers/poly_renderer.cpp renderers/poly_renderer.h
    renderers/tet_renderer.cpp renderers/tet_renderer.h
    renderers/hex_renderer.cpp renderers/hex_renderer.h

    models/model.h models/model.cpp
    models/polyline_model.h
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

target_include_directories(salamesh_core PUBLIC ${LUA_INCLUDE_DIR})
target_link_libraries(salamesh_core PUBLIC ${LUA_LIBRARIES})

if(WIN32)
  # Define a custom command to copy assets after the executable is built
  add_custom_command(
      TARGET ${PROJECT_NAME}
      POST_BUILD
      # Shared librairies copy
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/libs/lua-5.4.2_Win64_dll17_lib/lua54.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/
      COMMENT "Copying assets to ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/assets/"
  )
endif(WIN32)