file(GLOB_RECURSE GLAD_SOURCES "*.cpp" "*.h")

add_library(glad INTERFACE ${GLAD_SOURCES})

target_include_directories(glad INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)