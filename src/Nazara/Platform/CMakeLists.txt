get_filename_component(NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(NAME "Nazara${NAME}")

file(GLOB SRC LIST_DIRECTORIES false CONFIGURE_DEPENDS "*.cpp")

if(USE_SDL2)
    file(GLOB SRC_PLATFORM LIST_DIRECTORIES false CONFIGURE_DEPENDS "SDL2/*.cpp")
elseif(WIN32)
    file(GLOB SRC_PLATFORM LIST_DIRECTORIES false CONFIGURE_DEPENDS "Win32/*.cpp")
elseif(UNIX)
    file(GLOB SRC_PLATFORM LIST_DIRECTORIES false CONFIGURE_DEPENDS "X11/*.cpp")
else()
    message(SEND_ERROR "Unsuported platform")
endif()

add_library(${NAME} ${SRC} ${SRC_PLATFORM})

if(USE_SDL2)
    target_compile_definitions(${NAME} PRIVATE NAZARA_PLATFORM_SDL2 SDL_VIDEO_DRIVER_X11 SDL_VIDEO_DRIVER_WAYLAND)
    target_link_libraries(${NAME} PRIVATE SDL2 PUBLIC NazaraCore PUBLIC NazaraUtility)
endif()

target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_include_directories(${NAME} PRIVATE "${PROJECT_SOURCE_DIR}/thirdparty/include")
