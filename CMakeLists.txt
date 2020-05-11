cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(boost-ui LANGUAGES CXX)

option(UI_BUILD_TESTING "Build SelfTest project" ON)
option(UI_BUILD_EXAMPLE "Build documentation examples" ON)

add_subdirectory(sources)

if(UI_BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

if(UI_BUILD_EXAMPLE)
  add_subdirectory(examples)
endif()

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()
