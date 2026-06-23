file(GLOB_RECURSE SCENE_SOURCES "*.cpp" "*.h")

add_library(scene ${SCENE_SOURCES})

target_include_directories(scene
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(scene PUBLIC geometry materials utils)