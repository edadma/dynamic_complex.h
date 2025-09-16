cmake_minimum_required(VERSION 3.16)
project(dynamic_complex_h C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add include directories
include_directories(.)
include_directories(devDeps)
include_directories(devDeps/unity)

# Unity test framework source files
set(UNITY_SOURCES
    devDeps/unity/unity.c
)

# Main test executable
add_executable(tests
    main.c
    ${UNITY_SOURCES}
)

# Link math library for complex number operations
target_link_libraries(tests m)

# Enable testing
enable_testing()
add_test(NAME ComplexNumberTests COMMAND tests)

# Add compiler flags for better error reporting and Unity double precision
target_compile_options(tests PRIVATE -Wall -Wextra -g)
target_compile_definitions(tests PRIVATE UNITY_INCLUDE_DOUBLE)

# Optional: Add debug configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(tests PRIVATE DEBUG=1)
endif()

# Set executable name for main target (backwards compatibility)
add_executable(dynamic_complex_h main.c ${UNITY_SOURCES})
target_link_libraries(dynamic_complex_h m)
target_compile_options(dynamic_complex_h PRIVATE -Wall -Wextra -g)
target_compile_definitions(dynamic_complex_h PRIVATE UNITY_INCLUDE_DOUBLE)
