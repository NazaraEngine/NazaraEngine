get_filename_component(NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(NAME "${NAME}")

file(GLOB SRC LIST_DIRECTORIES false CONFIGURE_DEPENDS "*.cpp")

add_executable(${NAME} ${SRC})

target_link_libraries(${NAME}
    NazaraSDK
)
target_compile_definitions(${NAME} PRIVATE NAZARA_RENDERER_OPENGL NAZARA_AUDIO_OPENAL NAZARA_PLATFORM_SDL2)

target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../SDK/include")
target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/../include")
#target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
#target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/thirdparty/include")
