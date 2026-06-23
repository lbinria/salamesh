file(GLOB_RECURSE GEOMETRY_SOURCES "*.cpp" "*.h")

add_library(geometry ${GEOMETRY_SOURCES})

target_include_directories(geometry
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Optional: Link external libraries if needed
# target_link_libraries(materials PUBLIC some_library)