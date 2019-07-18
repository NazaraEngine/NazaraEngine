set(NAME "NazaraSDK")

file(GLOB_RECURSE SRC LIST_DIRECTORIES false CONFIGURE_DEPENDS "*.cpp")

add_library(${NAME} ${SRC})

target_link_libraries(${NAME}
    PUBLIC NazaraCore
    #PRIVATE NazaraPlatform
    PUBLIC NazaraAudio
    #PRIVATE NazaraGraphics
    PUBLIC NazaraPhysics2D
    PUBLIC NazaraPhysics3D
    PUBLIC NazaraRenderer
    PUBLIC NazaraNetwork
    PUBLIC NazaraLua
)

target_compile_definitions(${NAME} PRIVATE NDK_BUILD)

target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include"
                                   PRIVATE "${PROJECT_SOURCE_DIR}/src"
                                   PRIVATE "${PROJECT_SOURCE_DIR}/SDK/include"
                                   PRIVATE "${PROJECT_SOURCE_DIR}/SDK/src"
                                   PRIVATE "${PROJECT_SOURCE_DIR}/thirdparty/include"
)
