set(NAME "stb")

file(GLOB SRC LIST_DIRECTORIES false CONFIGURE_DEPENDS "*.cpp")

add_library(${NAME} ${SRC})

target_include_directories(${NAME} PRIVATE "../../include")
#target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
#target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/thirdparty/include")
#target_include_directories(${NAME} PRIVATE "${OPENGL_INCLUDE_DIR}")
