cmake_minimum_required(VERSION 4.0)
project(Boids)

set(CMAKE_CXX_STANDARD 20)

add_executable(Boids src/main.cpp
        src/boids.h
        src/boids.cpp
        src/grid.hpp
        src/settings.h
        src/settings.cpp
        src/stats.cpp
        src/stats.h)

add_subdirectory(lib/SDL-release-3.2.24 EXCLUDE_FROM_ALL)

target_link_libraries(Boids PRIVATE SDL3::SDL3)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /fp:fast /favor:INTEL64")

    if (USE_OPENMP)
        target_compile_options(Boids PRIVATE /openmp)
    endif()
endif()

# Copy SDL3 dll to boids executable folder (only in Windows)
if(WIN32)
    add_custom_command(
            TARGET Boids POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:Boids>
            VERBATIM
    )
endif()

# Copy settings.txt to boids executable folder
add_custom_command(
        TARGET Boids POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy ${CMAKE_SOURCE_DIR}/settings.txt $<TARGET_FILE_DIR:Boids>
        VERBATIM
)