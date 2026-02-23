cmake_minimum_required(VERSION 3.26...4.1)

project(
  livefind
  VERSION 0.3
  DESCRIPTION "Ncurses-based live directory search tool "
  LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

add_subdirectory(src)

