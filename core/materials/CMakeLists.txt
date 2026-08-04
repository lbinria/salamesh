file(GLOB_RECURSE MATERIALS_SOURCES "*.cpp" "*.h")

add_library(materials ${MATERIALS_SOURCES})

target_include_directories(materials
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(materials PUBLIC data rendering nlohmann_json)