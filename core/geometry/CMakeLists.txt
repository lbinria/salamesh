file(GLOB_RECURSE GEOMETRY_SOURCES "*.cpp" "*.h")

add_library(geometry ${GEOMETRY_SOURCES})

target_include_directories(geometry
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(geometry PUBLIC rendering data nlohmann_json utils)