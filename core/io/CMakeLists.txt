file(GLOB_RECURSE IO_SOURCES "*.cpp" "*.h")

add_library(io ${IO_SOURCES})

target_include_directories(io
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(io PUBLIC geometry materials)