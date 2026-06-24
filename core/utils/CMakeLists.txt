file(GLOB_RECURSE UTILS_SOURCES "*.cpp" "*.h")

add_library(utils ${UTILS_SOURCES})

target_include_directories(utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(utils PUBLIC stb_image)