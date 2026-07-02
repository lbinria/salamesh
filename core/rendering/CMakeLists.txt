file(GLOB_RECURSE SHADERS_SOURCES "*.cpp" "*.h")

add_library(rendering ${SHADERS_SOURCES})

target_include_directories(rendering
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rendering PUBLIC data glad)

if(WIN32)
    target_link_libraries(rendering PUBLIC glew)
endif(WIN32)