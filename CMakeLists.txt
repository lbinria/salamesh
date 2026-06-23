cmake_minimum_required(VERSION 3.10)

# Project configuration
project(salamesh LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT is_multi_config AND NOT (CMAKE_BUILD_TYPE OR DEFINED ENV{CMAKE_BUILD_TYPE}))
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Release default")
endif()

find_package(OpenMP COMPONENTS CXX)

if (MSVC)
  add_compile_options(/W4 /openmp:llvm   /wd26451 /bigobj)
else()
  # TODO activate when ready to that 
  # lots of warnings and all warnings as errors
  # add_compile_options(-Wall -Wextra -pedantic)
endif()

IF(WIN32)
  SET(PROJECT_BINARY_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

	add_subdirectory(include/glew)
	add_subdirectory(include/glfw)

  set(OPENGL_FILES)

	SET(OPENGL_LIBS
		opengl32
		glew
	)

ELSE()
	# find_package(OpenGL REQUIRED)
  find_package(glfw3 REQUIRED)
  set(OPENGL_FILES include/glad.c)

	SET(OPENGL_LIBS
		${OPENGL_LIBRARIES}
	)
ENDIF(WIN32)

set(GLM_INCLUDE_DIR "include")
include_directories(${GLM_INCLUDE_DIR})

# ULTIMAILLE

# Get development version of ultimaille directly from git repository 
include(FetchContent)
FetchContent_Declare(
  ultimaille
  GIT_REPOSITORY https://github.com/ssloy/ultimaille
  GIT_TAG        bed2ad0
)
FetchContent_MakeAvailable(ultimaille)


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

# Create an IMGUI static target (so its objects are linked into salamesh_core and consumers)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui
  ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/backends
)

# Link any backend deps if needed (glfw, OpenGL libs) as PRIVATE or PUBLIC appropriately:
target_link_libraries(imgui PRIVATE glfw ${OPENGL_LIBS})

# set(JSON_INCLUDE_DIR "include/json")

# set(JSON_SOURCES
#     ${JSON_INCLUDE_DIR}/json.hpp
# )

# set(STB_INCLUDE_DIR "include/stb_image")

# set(STB_SOURCES
#   ${STB_INCLUDE_DIR}/stb_include.cpp
# )

add_subdirectory(include/ImGuiFileDialog-0.6.7)
get_target_property(_IFD_INCS ImGuiFileDialog INTERFACE_INCLUDE_DIRECTORIES)
if(_IFD_INCS)
  target_include_directories(ImGuiFileDialog
    SYSTEM INTERFACE
      ${_IFD_INCS}
  )
endif()

file(GLOB SOURCES 
  "core/*.h" 
  "core/*.cpp"
  "core/**/*.h" 
  "core/**/*.cpp"
)



# Add the library
add_library(salamesh_core SHARED
  # some include files to build with the executable
  ${SOURCES}
  ${OPENGL_FILES}
  ${SOL_SOURCES}
  # ${IMGUI_SOURCES}
  # ${JSON_SOURCES}
  # ${STB_SOURCES}
)

# Move include directories to the library target
target_include_directories(salamesh_core PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    include 
    include/imgui
    ${LUA_INCLUDE_DIR}
    ${ultimaille_SOURCE_DIR}
    # ${JSON_INCLUDE_DIR}
    # ${STB_INCLUDE_DIR}
)


target_include_directories(salamesh_core PRIVATE 
    ${stlbfgs_SOURCE_DIR}
    ${OpenNL_psm_SOURCE_DIR}
)

# Move the dependencies to the library
target_link_libraries(salamesh_core PUBLIC 
    glfw  
    ultimaille 
    ${LUA_LIBRARIES}
    ${OPENGL_LIBS}
    $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>
    imgui
)

# Local libs
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/cameras)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/geometry)
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/shaders)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/materials)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/scripting)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/data)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/utils)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core/io)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include/json)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/include/stb_image)


# Link libraries to main executable
target_link_libraries(salamesh_core
  PUBLIC
      cameras
      geometry
      materials
      scripting
      data
      utils
      io
      # shaders

      nlohmann_json
      stb_image
)

if (NOT WIN32)
    target_link_libraries(salamesh_core PRIVATE m)
endif()


file(GLOB APP_SOURCES 
  "*.h" 
  "*.cpp"
  "bindings/*.h" 
  "bindings/*.cpp"
  "bindings/**/*.h" 
  "bindings/**/*.cpp"
)

# Add the executable
add_executable(salamesh ${APP_SOURCES})
# Link the executable to your core library
target_link_libraries(salamesh PRIVATE salamesh_core ImGuiFileDialog)




if (NOT WIN32)
target_link_libraries(salamesh PRIVATE m)
endif()


message("Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")

message("Copying assets from ${PROJECT_SOURCE_DIR}/assets to ${CMAKE_CURRENT_BINARY_DIR}/${CONFIGURATION}/assets")
file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message("Copying shaders from ${PROJECT_SOURCE_DIR}/shaders to ${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(COPY ${PROJECT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message("Copying default / test modules from ${PROJECT_SOURCE_DIR}/modules to ${CMAKE_CURRENT_BINARY_DIR}/modules")
file(COPY ${PROJECT_SOURCE_DIR}/modules DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message("Copying default settings from ${PROJECT_SOURCE_DIR}/settings.json to ${CMAKE_CURRENT_BINARY_DIR}/settings.json")
configure_file(${PROJECT_SOURCE_DIR}/settings.json ${CMAKE_CURRENT_BINARY_DIR}/settings.json COPYONLY)

if(MSVC)
# Define a custom command to copy assets after the executable is built
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    # Shared librairies copy
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/libs/lua-5.4.2_Win64_dll17_lib/lua54.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/include/glew/bin/Release/x64/glew32.dll ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/
    # Default settings copy
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/settings.json ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/
    # Shader copy
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/shaders
    # COMMENT "Copying shaders to ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/shaders/"
    # Modules copy
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/modules
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/modules ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/modules
    # COMMENT "Copying modules to ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/modules/"  
    # Assets copy
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/assets
    COMMENT "Copying assets to ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/assets/"
)
endif()