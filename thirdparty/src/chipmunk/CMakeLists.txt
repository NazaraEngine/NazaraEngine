set(NAME "chipmunk")

file(GLOB_RECURSE SRC LIST_DIRECTORIES false CONFIGURE_DEPENDS "*.c")

add_library(${NAME} ${SRC})

target_include_directories(${NAME} PRIVATE "../../include")
