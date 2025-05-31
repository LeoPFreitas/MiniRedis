# This file contains a list of tools that can be activated and downloaded on-demand
# Each tool is enabled during configuration by passing an additional `-DUSE_<TOOL>=<VALUE>` argument to CMake

# Only activate tools for top level project
if (NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif ()

include(${CMAKE_CURRENT_LIST_DIR}/CPM.cmake)

# Enhanced warnings configuration
include(CheckCXXCompilerFlag)
function(enable_cxx_compiler_flag_if_supported flag)
  string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
  if (flag_already_set EQUAL -1)
    check_cxx_compiler_flag("${flag}" flag_supported)
    if (flag_supported)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
    endif ()
  endif ()
endfunction()

# Add warnings based on compiler
if (MSVC)
  enable_cxx_compiler_flag_if_supported("/W4")
  enable_cxx_compiler_flag_if_supported("/permissive-")
else ()
  enable_cxx_compiler_flag_if_supported("-Wall")
  enable_cxx_compiler_flag_if_supported("-Wextra")
  enable_cxx_compiler_flag_if_supported("-Wpedantic")
  enable_cxx_compiler_flag_if_supported("-Werror")
endif ()

# Enables sanitizers support using the USE_SANITIZER flag
# Available values are: Address, Memory, MemoryWithOrigins, Undefined, Thread, Leak, 'Address;Undefined'
if (USE_SANITIZER OR USE_STATIC_ANALYZER)
  CPMAddPackage("gh:StableCoder/cmake-scripts#24.08.1")

  if (USE_SANITIZER)
    include(${cmake-scripts_SOURCE_DIR}/sanitizers.cmake)
  endif ()

  if (USE_STATIC_ANALYZER)
    if ("clang-tidy" IN_LIST USE_STATIC_ANALYZER)
      set(CLANG_TIDY ON CACHE INTERNAL "")
    else ()
      set(CLANG_TIDY OFF CACHE INTERNAL "")
    endif ()
    if ("iwyu" IN_LIST USE_STATIC_ANALYZER)
      set(IWYU ON CACHE INTERNAL "")
    else ()
      set(IWYU OFF CACHE INTERNAL "")
    endif ()
    if ("cppcheck" IN_LIST USE_STATIC_ANALYZER)
      set(CPPCHECK ON CACHE INTERNAL "")
    else ()
      set(CPPCHECK OFF CACHE INTERNAL "")
    endif ()

    include(${cmake-scripts_SOURCE_DIR}/tools.cmake)

    if (${CLANG_TIDY})
      clang_tidy(${CLANG_TIDY_ARGS})
    endif ()

    if (${IWYU})
      include_what_you_use(${IWYU_ARGS})
    endif ()

    if (${CPPCHECK})
      cppcheck(${CPPCHECK_ARGS})
    endif ()
  endif ()
endif ()

# Enables CCACHE support through the USE_CCACHE flag
# Possible values are: YES, NO or equivalent
if (USE_CCACHE)
  CPMAddPackage("gh:TheLartians/Ccache.cmake@1.2.5")
endif ()

# Code coverage support
if (USE_COVERAGE)
  CPMAddPackage("gh:StableCoder/cmake-scripts#24.08.1")
  include(${cmake-scripts_SOURCE_DIR}/code-coverage.cmake)
  add_code_coverage()
endif ()

# Format checking with clang-format
if (USE_FORMAT)
  CPMAddPackage("gh:TheLartians/Format.cmake@1.8.3")
  add_format_target(${PROJECT_NAME})
endif ()

# Version checking for critical tools
find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
  execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
      OUTPUT_VARIABLE GIT_SHA
      ERROR_QUIET
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  add_definitions(-DGIT_SHA="${GIT_SHA}")
endif ()

# Set default values for sanitizers if not specified
if (NOT DEFINED SANITIZER_BLACKLIST)
  set(SANITIZER_BLACKLIST "${PROJECT_SOURCE_DIR}/sanitizer-blacklist.txt")
endif ()

# Configure benchmark settings
option(BUILD_BENCHMARK "Build the benchmark targets" OFF)
if (BUILD_BENCHMARK)
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  CPMAddPackage(
      NAME benchmark
      GITHUB_REPOSITORY google/benchmark
      VERSION 1.8.3
      OPTIONS "BENCHMARK_ENABLE_TESTING OFF"
  )
endif ()