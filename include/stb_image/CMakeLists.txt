file(GLOB_RECURSE STB_IMAGE_SOURCES "*.cpp" "*.h")

add_library(stb_image ${STB_IMAGE_SOURCES})

target_include_directories(stb_image PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)