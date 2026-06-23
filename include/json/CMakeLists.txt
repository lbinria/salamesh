file(GLOB_RECURSE JSON_SOURCES "*.cpp" "*.h")

add_library(nlohmann_json INTERFACE ${JSON_SOURCES})

target_include_directories(nlohmann_json INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Optional: Link external libraries if needed
# target_link_libraries(nlohmann_json PUBLIC data geometry materials json)