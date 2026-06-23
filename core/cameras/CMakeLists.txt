file(GLOB_RECURSE CAMERAS_SOURCES "*.cpp" "*.h")

add_library(cameras ${CAMERAS_SOURCES})

target_include_directories(cameras
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Optional: Link external libraries if needed
# target_link_libraries(cameras PUBLIC some_library)