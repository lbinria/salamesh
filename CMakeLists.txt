cmake_minimum_required(VERSION 3.10)

project(salamesh)

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

# ULTIMAILLE

# Get development version of ultimaille directly from git repository 
include(FetchContent)
FetchContent_Declare(
  ultimaille
  GIT_REPOSITORY https://github.com/ssloy/ultimaille
  GIT_TAG        v2.0.0
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

# Add the executable
add_executable(salamesh 
  main.cpp 
  app.cpp app.h
  my_app.cpp my_app.h
  shader.cpp shader.h 
  tri_mesh.cpp tri_mesh.h 
  hex_mesh.cpp hex_mesh.h 
  points_cloud.cpp points_cloud.h
  wireframe.cpp wireframe.h
  arcball_camera.h 
  tool.h hex_collapse_tool.h hex_collapse_tool.cpp debug_tool.h debug_tool.cpp
  lua_console.h lua_console.cpp
  lua_engine.h lua_engine.cpp
  component.h
  settings_manager.h
  stb_include.cpp 
  glad.c 
  ${SOL_SOURCES}
  ${IMGUI_SOURCES}
  ${JSON_SOURCES}
)
# add_executable(salamesh main.cpp glad.c)

target_include_directories(salamesh PRIVATE include)
target_include_directories(salamesh PRIVATE include/imgui)

target_include_directories(salamesh PRIVATE ${LUA_INCLUDE_DIR})

# Link the libraries
target_link_libraries(salamesh PRIVATE glfw)
target_link_libraries(salamesh PRIVATE ImGuiFileDialog ${CMAKE_DL_LIBS} ultimaille m $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(salamesh PRIVATE ${LUA_LIBRARIES})