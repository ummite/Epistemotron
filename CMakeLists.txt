cmake_minimum_required(VERSION 3.16)
project(EpistemotronSimulation
    VERSION 1.0.0
    DESCRIPTION "N-body gravitational simulation library"
    LANGUAGES CXX
)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build options
option(BUILD_TESTS "Build test programs" ON)
option(BUILD_EXAMPLES "Build example programs" OFF)

# ============================================================================
# Library
# ============================================================================

add_library(epistemotron
    Simulation/Mass.cpp
    Simulation/Universe.cpp
    Simulation/Simulator.cpp
)

target_include_directories(epistemotron
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(epistemotron PUBLIC cxx_std_17)

# Install targets
install(TARGETS epistemotron
    EXPORT EpistemotronTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY Simulation/
    DESTINATION include/Simulation
    FILES_MATCHING PATTERN "*.h"
)

# ============================================================================
# Test executable
# ============================================================================

if(BUILD_TESTS)
    add_executable(test_simulation
        tests/main.cpp
    )

    target_link_libraries(test_simulation PRIVATE epistemotron)

    # Add test to CTest
    include(CTest)
    add_test(NAME EpistemotronTests COMMAND test_simulation)
endif()

# ============================================================================
# Example: Simple demo program
# ============================================================================

if(BUILD_EXAMPLES)
    add_executable(example_basic
        examples/basic.cpp
    )

    target_link_libraries(example_basic PRIVATE epistemotron)
endif()

# ============================================================================
# Documentation
# ============================================================================

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_TREEVIEW YES)

    doxygen_add_docs(docs
        Simulation
        COMMENT "Generating API documentation with Doxygen"
    )
endif()

# ============================================================================
# Print configuration summary
# ============================================================================

message(STATUS "")
message(STATUS "EpistemotronSimulation ${PROJECT_VERSION}")
message(STATUS "==========================================")
message(STATUS "Configuration:")
message(STATUS "  CMake version: ${CMAKE_VERSION}")
message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
if(BUILD_TESTS)
    message(STATUS "  Test executable: ${CMAKE_BINARY_DIR}/bin/test_simulation")
endif()
message(STATUS "")