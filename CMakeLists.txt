cmake_minimum_required(VERSION 3.0.0)

set (PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
  message("Add define DEBUGGING")
  add_definitions(-DDEBUGGING)
  set(PROJECT_NAME "apriori_debug")
else()
  set(PROJECT_NAME "apriori")
endif()

# config here
set(SOURCE_FOLDER "src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# set version here
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_FIX 0)
configure_file (
  "${PROJECT_SOURCE_DIR}/${SOURCE_FOLDER}/version.h.in"
  "${PROJECT_SOURCE_DIR}/${SOURCE_FOLDER}/version.h"
)

# the included folder for <foo.h>
include_directories(${PROJECT_SOURCE_DIR})
link_directories("libcsv/.libs/")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # using Clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using gcc
  SET(CMAKE_C_COMPILER g++)
  set(WARNING "-Wall")
  set(PUBLIC "-static -static-libgcc -static-libg++")
  set(IDENTIFIER "-fextended-identifiers --input-charset=utf-8 -finput-charset=utf-8 --exec-charset=utf-8")
  set(MULTITHREAD "-pthread")
  set(CPPVERSION "-std=c++11")
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
    ${WARNING} \
    ${IDENTIFIER} \
    ${CPPVERSION} \
    ${MULTITHREAD}"
  )
  # add_subdirectory("src")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C Compiler
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
endif()

set (SOURCE
  src/main.cpp
  src/readcsv.cpp
  )

add_executable (${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} "csv")
