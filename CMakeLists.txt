cmake_minimum_required(VERSION 3.12...3.26)

project(salamesh)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)


# Find the GLFW library
find_package(glfw3 REQUIRED)

find_package(Lua REQUIRED)

# # Clone ImGUI
# include(FetchContent)
# FetchContent_Declare(
#   imgui
#   GIT_REPOSITORY https://github.com/ocornut/imgui.git
#   GIT_TAG        v1.91.5
# )
# FetchContent_MakeAvailable(imgui)
# include_directories(${imgui_SOURCE_DIR})

set(IMGUI_SOURCES
    include/imgui/imgui.cpp
    include/imgui/imgui.h
    
    include/imgui/imgui_demo.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui_tables.cpp

    include/imgui/backends/imgui_impl_glfw.h
    include/imgui/backends/imgui_impl_glfw.cpp
    include/imgui/backends/imgui_impl_opengl3.h
    include/imgui/backends/imgui_impl_opengl3.cpp
    include/imgui/backends/imgui_impl_opengl3_loader.h
)

add_subdirectory(include/ImGuiFileDialog-0.6.7)
get_target_property(_IFD_INCS ImGuiFileDialog INTERFACE_INCLUDE_DIRECTORIES)
if(_IFD_INCS)
  target_include_directories(ImGuiFileDialog
    SYSTEM INTERFACE
      ${_IFD_INCS}
  )
endif()

# ULTIMAILLE

# Get development version of ultimaille directly from git repository 
include(FetchContent)
FetchContent_Declare(
  ultimaille
  GIT_REPOSITORY https://github.com/ssloy/ultimaille
  GIT_TAG        8fb23b7
)
FetchContent_MakeAvailable(ultimaille)

# Add ultimaille directory to get ultimaille CMakeLists
include_directories(${ultimaille_SOURCE_DIR})

set(SOL_SOURCES
    include/sol/sol.hpp
    include/sol/config.hpp
    include/sol/forward.hpp
)

set(JSON_SOURCES
    include/json.hpp
)

add_subdirectory(core)

# Add the executable
add_executable(salamesh 
  main.cpp 
  app.cpp app.h
  my_app.cpp my_app.h
  script.cpp script.h
  shader.cpp shader.h 
  point_set_renderer.cpp point_set_renderer.h
  halfedge_renderer.cpp halfedge_renderer.h
  tri_renderer.cpp tri_renderer.h
  quad_renderer.cpp quad_renderer.h
  tet_renderer.cpp tet_renderer.h
  hex_renderer.cpp hex_renderer.h
  tri_model.cpp tri_model.h
  quad_model.cpp quad_model.h
  tet_model.cpp tet_model.h
  hex_model.cpp hex_model.h
  helpers/module_loader.h
  helpers/settings_manager.h
  cameras/arcball_camera.h
  cameras/descent_camera.h
  render_surface.cpp render_surface.h
  # some include files to build with the executable
  include/stb_include.cpp 
  include/glad.c 
  ${SOL_SOURCES}
  ${IMGUI_SOURCES}
  ${JSON_SOURCES}
)

target_include_directories(salamesh PRIVATE include)
target_include_directories(salamesh PRIVATE include/imgui)

target_include_directories(salamesh PRIVATE ${LUA_INCLUDE_DIR})

# Link the libraries
target_link_libraries(salamesh PRIVATE salamesh_core)
target_link_libraries(salamesh PRIVATE glfw)
target_link_libraries(salamesh PRIVATE ImGuiFileDialog ${CMAKE_DL_LIBS} ultimaille m $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(salamesh PRIVATE ${LUA_LIBRARIES})

# Copy assets to the build directory
message("Copying assets from ${PROJECT_SOURCE_DIR}/assets to ${CMAKE_CURRENT_BINARY_DIR}/assets")
file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
message("Copying shaders from ${PROJECT_SOURCE_DIR}/shaders to ${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(COPY ${PROJECT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})
message("Copying default / test modules from ${PROJECT_SOURCE_DIR}/modules to ${CMAKE_CURRENT_BINARY_DIR}/modules")
file(COPY ${PROJECT_SOURCE_DIR}/modules DESTINATION ${CMAKE_BINARY_DIR})
message("Copying default settings from ${PROJECT_SOURCE_DIR}/settings.json to ${CMAKE_CURRENT_BINARY_DIR}/settings.json")
configure_file(${PROJECT_SOURCE_DIR}/settings.json ${CMAKE_BINARY_DIR}/settings.json COPYONLY)