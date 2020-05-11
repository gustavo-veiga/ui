cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(ui LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# wxWigets
find_package(wxWidgets REQUIRED net gl core base)
include(${wxWidgets_USE_FILE})

# Boost
find_package(Boost 1.67 REQUIRED COMPONENTS core config function bind move optional range)
include_directories(${Boost_INCLUDE_DIRS})

# Library
file(GLOB FILES ../include/**.hpp **.cpp)
add_library(${PROJECT_NAME} STATIC ${FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ../include)
target_link_libraries(${PROJECT_NAME} PUBLIC ${wxWidgets_LIBRARIES} ${Boost_LIBRARIES})
