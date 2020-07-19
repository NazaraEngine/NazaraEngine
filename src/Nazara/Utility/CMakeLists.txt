get_filename_component(NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(NAME "Nazara${NAME}")

file(GLOB_RECURSE SRC LIST_DIRECTORIES false CONFIGURE_DEPENDS "*.cpp")

add_library(${NAME} ${SRC} ${SRC_PLATFORM})

if(NOT EMSCRIPTEN)
    target_link_libraries(${NAME}
        PRIVATE stb
        PRIVATE Freetype::Freetype
    )
else()
    target_link_libraries(${NAME}
        PRIVATE stb
    )
endif()

target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/thirdparty/include")
