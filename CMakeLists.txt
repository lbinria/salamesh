cmake_minimum_required(VERSION 3.5...4.1.2)

project(salamesh)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

find_package(OpenMP COMPONENTS CXX)

if (MSVC)
  add_compile_options(/W4 /openmp:llvm   /wd26451 /bigobj)
endif()

IF(WIN32)
  SET(PROJECT_BINARY_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

	add_subdirectory(include/glew)
	add_subdirectory(include/glfw)

  # Set the path to the Lua library and include directory
  set(LUA_DIR "${CMAKE_SOURCE_DIR}/libs/lua-5.4.2_Win64_dll17_lib")
  set(LUA_INCLUDE_DIR "${LUA_DIR}/include")
  set(LUA_LIB_DIR "${LUA_DIR}")

  # Include the lib directory
  include_directories(${LUA_INCLUDE_DIR})
  # Link against the Lua library
  # find_library(LUA_LIBRARIES lua5.4 HINTS ${LUA_LIB_DIR})
  # Directly link the Lua library
  set(LUA_LIBRARIES "${LUA_LIB_DIR}/lua54.lib") # Change this to the actual library filename

  # Ensure that the library was found
  if(NOT LUA_LIBRARIES)
      message(FATAL_ERROR "Could not find Lua library")
  endif()

  set(OPENGL_FILES)

	SET(OPENGL_LIBS
		opengl32
		glew
	)

ELSE()
	# FIND_PACKAGE(OpenGL REQUIRED)
  find_package(glfw3 REQUIRED)
  find_package(Lua REQUIRED)
  set(OPENGL_FILES include/glad.c)

	SET(OPENGL_LIBS
		${OPENGL_LIBRARIES}
	)
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
  GIT_TAG        c6647c059ea952112c4ddb6613f5224e636c74db
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



  # helpers/module_loader.h
  # helpers/settings_manager.h
  cameras/arcball_camera.h
  cameras/descent_camera.h
  render_surface.cpp render_surface.h
  # some include files to build with the executable
  include/stb_include.cpp 
  ${OPENGL_FILES}
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
target_link_libraries(salamesh PRIVATE ImGuiFileDialog ${CMAKE_DL_LIBS} ultimaille $<$<BOOL:${OpenMP_CXX_FOUND}>:OpenMP::OpenMP_CXX>)
target_link_libraries(salamesh PRIVATE ${LUA_LIBRARIES})
target_link_libraries(salamesh PRIVATE ${OPENGL_LIBS})

if (NOT WIN32)
target_link_libraries(salamesh PRIVATE m)
endif()

if(MSVC)
    set(CONFIGURATION ${CMAKE_CFG_INTDIR})  # MSVC uses CMAKE_CFG_INTDIR
else()
    set(CONFIGURATION "")  # Default behavior for other systems
endif()

# Copy assets to the build directory
# message("Configuration: ${CMAKE_CFG_INTDIR}")

# Listing all output configs
# foreach (OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
# message("Configuration: ${OUTPUTCONFIG}")
# endforeach (OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

message("Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")

message("Copying assets from ${PROJECT_SOURCE_DIR}/assets to ${CMAKE_CURRENT_BINARY_DIR}/${CONFIGURATION}/assets")
file(COPY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message("Copying shaders from ${PROJECT_SOURCE_DIR}/shaders to ${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(COPY ${PROJECT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message("Copying default / test modules from ${PROJECT_SOURCE_DIR}/modules to ${CMAKE_CURRENT_BINARY_DIR}/modules")
file(COPY ${PROJECT_SOURCE_DIR}/modules DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
message("Copying default settings from ${PROJECT_SOURCE_DIR}/settings.json to ${CMAKE_CURRENT_BINARY_DIR}/settings.json")
configure_file(${PROJECT_SOURCE_DIR}/settings.json ${CMAKE_CURRENT_BINARY_DIR}/settings.json COPYONLY)


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
