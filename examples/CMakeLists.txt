project(NazaraEngineExamples LANGUAGES CXX)

file(GLOB EXAMPLE_DIRS LIST_DIRECTORIES true *)

list(FILTER EXAMPLE_DIRS EXCLUDE REGEX "/bin|CMakeLists.txt")

foreach(DIR IN LISTS EXAMPLE_DIRS)
    add_subdirectory(${DIR})
endforeach()
