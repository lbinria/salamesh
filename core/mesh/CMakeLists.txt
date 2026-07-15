file(GLOB_RECURSE GEOMETRY_SOURCES "*.cpp" "*.h")

add_library(mesh ${GEOMETRY_SOURCES})

target_include_directories(mesh
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mesh PUBLIC rendering data nlohmann_json utils)