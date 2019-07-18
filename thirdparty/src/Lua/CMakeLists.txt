set(NAME "Lua")

file(GLOB SRC LIST_DIRECTORIES false CONFIGURE_DEPENDS "*.cpp")

add_library(${NAME} ${SRC})

target_include_directories(${NAME} PRIVATE "../../include")
