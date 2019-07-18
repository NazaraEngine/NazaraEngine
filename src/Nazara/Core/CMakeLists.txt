get_filename_component(NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(NAME "Nazara${NAME}")

file(GLOB SRC LIST_DIRECTORIES false CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE SRC_HASH LIST_DIRECTORIES false CONFIGURE_DEPENDS "Hash/*.cpp")
file(GLOB SRC_DEBUG LIST_DIRECTORIES false CONFIGURE_DEPENDS "Debug/*.cpp")

if(WIN32)
    file(GLOB SRC_PLATFORM LIST_DIRECTORIES false CONFIGURE_DEPENDS "Win32/*.cpp")
else(WIN32)
    file(GLOB SRC_PLATFORM LIST_DIRECTORIES false CONFIGURE_DEPENDS "Posix/*.cpp")
endif(WIN32)

add_library(${NAME} ${SRC} ${SRC_PLATFORM} ${SRC_HASH} ${SRC_DEBUG})

target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/thirdparty/include")
