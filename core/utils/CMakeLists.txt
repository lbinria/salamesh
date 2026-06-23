file(GLOB_RECURSE UTILS_SOURCES "*.cpp" "*.h")

add_library(utils ${UTILS_SOURCES})

target_include_directories(utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Optional: Link external libraries if needed
# target_link_libraries(scripting PUBLIC some_library)