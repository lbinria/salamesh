file(GLOB_RECURSE DATA_SOURCES "*.cpp" "*.h")

add_library(data ${DATA_SOURCES})

target_include_directories(data
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ultimaille_SOURCE_DIR}  # Add this explicitly
)

target_link_libraries(data PUBLIC ultimaille)
