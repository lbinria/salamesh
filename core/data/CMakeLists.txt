file(GLOB_RECURSE DATA_SOURCES "*.cpp" "*.h")

add_library(data ${DATA_SOURCES})

target_include_directories(data
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Optional: Link external libraries if needed
# target_link_libraries(data PUBLIC some_library)