cmake_minimum_required(VERSION 3.31)

project(
    MiniRedis
    VERSION 0.1.0
    DESCRIPTION "A minimal Redis server implementation in C++"
    LANGUAGES CXX)

# ---- Include guards ----
if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
      FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif ()

# ---- Add dependencies with CPM ----
include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add coverage flags if build type is coverage
set(CMAKE_CONFIGURATION_TYPES "Coverage;${CMAKE_CONFIGURATION_TYPES}")
if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
  message(STATUS "Setting coverage build flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif ()

# Set the build type to Release by default
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  message(STATUS "Setting build type to '${CMAKE_BUILD_TYPE}' as none was specified.")
endif ()

# ---- Set output directories ----
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---- Library configuration ----
add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# ---- Add header files ----
target_sources(${PROJECT_NAME}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
    include/miniredis/miniredis.hpp
)

# ---- Add source files ----
target_sources(${PROJECT_NAME}
    PRIVATE
    src/miniredis.cpp
)

# ---- Set library properties ----
set_target_properties(${PROJECT_NAME}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME ${PROJECT_NAME}
)

# Apply coverage flags only to your library
if (ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Coverage")
  target_compile_options(${PROJECT_NAME} PRIVATE -O0 -g --coverage)
  target_link_options(${PROJECT_NAME} PRIVATE --coverage)
endif ()

# ---- Set include directories ----
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)

# --- Import tools ----
include(cmake/tools.cmake)

# ---- Coverage configuration ----
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# Add coverage flags if build type is coverage
set(CMAKE_CONFIGURATION_TYPES "Coverage;${CMAKE_CONFIGURATION_TYPES}")
if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
  message(STATUS "Setting coverage build flags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif ()

if (ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  message(STATUS "Enabling coverage reporting")

  # Find required tools
  find_program(GCOVR_PATH gcovr REQUIRED)

  # Add coverage targets
  add_custom_target(coverage
      # Create folder
      COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/coverage

      # Running gcovr with comprehensive exclusions
      COMMAND ${GCOVR_PATH}
      --root ${PROJECT_SOURCE_DIR}
      --exclude-throw-branches
      --exclude "_deps"
      --exclude "test/src/*"
      --exclude "Tests"
      --exclude ".*test.*"
      --html --html-details
      --output ${PROJECT_BINARY_DIR}/coverage/index.html
      --xml-pretty
      --xml ${PROJECT_BINARY_DIR}/coverage/coverage.xml

      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      DEPENDS MiniRedisTests
      COMMENT "Running gcovr to produce HTML coverage report"
  )

  add_custom_target(coverage_xml
      COMMAND ${GCOVR_PATH}
      --root ${PROJECT_SOURCE_DIR}
      --exclude-throw-branches
      --exclude "_deps"
      --exclude "test/src/*"
      --exclude "Tests"
      --exclude ".*test.*"
      --xml-pretty
      --output ${PROJECT_BINARY_DIR}/coverage.xml
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      DEPENDS MiniRedisTests
      COMMENT "Running gcovr to produce XML coverage report"
  )

  add_custom_target(coverage_summary
      COMMAND ${GCOVR_PATH}
      --root ${PROJECT_SOURCE_DIR}
      --exclude-throw-branches
      --exclude "_deps"
      --exclude "test/src/*"
      --exclude "Tests"
      --exclude ".*test.*"
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      DEPENDS MiniRedisTests
      COMMENT "Running gcovr to show coverage summary"
  )
endif ()

# ---- Tests ----
option(BUILD_TESTS "Build the tests" ON)

if (BUILD_TESTS)
  # Add GTest
  CPMAddPackage(
      NAME GTest
      GITHUB_REPOSITORY google/googletest
      GIT_TAG v1.17.0
      VERSION 1.17.0
      OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
  )

  # Create test executable
  file(GLOB test_sources CONFIGURE_DEPENDS test/src/*.cpp)
  add_executable(MiniRedisTests ${test_sources})
  target_link_libraries(MiniRedisTests PRIVATE GTest::gtest_main ${PROJECT_NAME}::${PROJECT_NAME})
  set_target_properties(MiniRedisTests PROPERTIES CXX_STANDARD 23)

  # Apply coverage flags to library
  if (ENABLE_COVERAGE)
    target_compile_options(${PROJECT_NAME} PRIVATE -O0 -g --coverage)
    target_link_options(${PROJECT_NAME} PRIVATE --coverage)
  endif ()

  # Apply coverage flags to test executable
  if (ENABLE_COVERAGE)
    target_compile_options(MiniRedisTests PRIVATE -O0 -g --coverage)
    target_link_options(MiniRedisTests PRIVATE --coverage)
  endif ()

  # Enable testing
  enable_testing()
  include(GoogleTest)
  gtest_discover_tests(MiniRedisTests)
endif ()