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

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)