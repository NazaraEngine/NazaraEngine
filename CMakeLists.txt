cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE ON)
project(NazaraEngine VERSION 0.5
                     LANGUAGES CXX C)


set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/CMake")

include(GNUInstallDirs)
include(FetchContent)
include(ExternalProject)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -fPIC -fsanitize=address,undefined")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_compile_options(-msse3)

# Set default build type to Debug
if(NOT DEFINED CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 14)

set(OpenGL_GL_PREFERENCE "GLVND")

set(BUILD_SHARED_LIBS ON)
set(USE_SDL2 ON)

include(FetchNazaraLibs)

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
#find_package(Lua REQUIRED)
if(NOT EMSCRIPTEN)
find_package(Freetype REQUIRED)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}
        SHELL:-s USE_SDL=2
        SHELL:-s USE_FREETYPE=1")
    # find_package(Freetype REQUIRED)
endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=cfi -flto -fvisibility=default")

SET(NAZARA_LIBS Core Platform Utility Audio Graphics Lua Network Noise Physics2D Physics3D Renderer)

add_subdirectory("thirdparty/src")

foreach(LIB IN LISTS NAZARA_LIBS)
    add_subdirectory("src/Nazara/${LIB}")
endforeach()

add_subdirectory("SDK/src/NDK")
add_subdirectory("examples")
