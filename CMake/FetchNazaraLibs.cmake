option(FETCH_SDL2 "Fetch and build SDL2 lib instead of serching for a system installed one" OFF)
option(FETCH_NEWTON "Fetch and build Newton lib instead of serching for a system installed one" ON)


if(USE_SDL2 AND FETCH_SDL2)
    FetchContent_Declare(
      sdl2
      URL      https://www.libsdl.org/release/SDL2-2.0.9.tar.gz
      URL_HASH MD5=f2ecfba915c54f7200f504d8b48a5dfe
      SOURCE_DIR sdl2
    )
    FetchContent_Populate(sdl2)
    add_subdirectory(${sdl2_SOURCE_DIR} ${sdl2_BINARY_DIR} EXCLUDE_FROM_ALL)
else()
    find_package(SDL2 REQUIRED)
endif()


if(FETCH_NEWTON)
    set(NEWTON_BUILD_CORE_ONLY ON)
    set(NEWTON_DEMOS_SANDBOX OFF)

    FetchContent_Declare(
      newton
      GIT_REPOSITORY    https://github.com/MADEAPPS/newton-dynamics.git
      GIT_TAG           newton-3.13
      SOURCE_DIR        newton
      BINARY_DIR        newton
    )
    FetchContent_Populate(newton)
    add_subdirectory(${newton_SOURCE_DIR}/coreLibrary_300 ${newton_BINARY_DIR}/coreLibrary_300 EXCLUDE_FROM_ALL)
else()
    find_package(Newton REQUIRED)
endif()
