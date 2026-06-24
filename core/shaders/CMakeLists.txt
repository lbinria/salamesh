file(GLOB_RECURSE SHADERS_SOURCES "*.cpp" "*.h")

add_library(shaders ${SHADERS_SOURCES})

target_include_directories(shaders
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Optional: Link external libraries if needed
target_link_libraries(shaders PUBLIC data geometry materials nlohmann_json utils)