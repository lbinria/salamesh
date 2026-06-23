file(GLOB_RECURSE SCRIPTING_SOURCES "*.cpp" "*.h")

add_library(scripting ${SCRIPTING_SOURCES})

target_include_directories(scripting
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Optional: Link external libraries if needed
# target_link_libraries(scripting PUBLIC some_library)