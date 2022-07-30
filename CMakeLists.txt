cmake_minimum_required(VERSION 3.12)

project(server)

set(CMAKE_CXX_STANDARD 17)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

option(BUILD_TESTS "Build unit tests." ON)
option(DOWNLOAD_THIRDPARTY "Download thirdparty." ON)

# 设置基础变量
SET(BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 链接interface 便于开发
INCLUDE(cmake/interface.cmake)

set(SERVER_MAJOR_VERSION 0)
set(SERVER_MINOR_VERSION 0)
set(SERVER_PATCH_VERSION 1)
set(SERVER_VERSION
    ${SERVER_MAJOR_VERSION}.${SERVER_MINOR_VERSION}.${SERVER_PATCH_VERSION})
set(SERVER_VERSION_STRING "${SERVER_VERSION}")

if(DOWNLOAD_THIRDPARTY) 
include(cmake/thirdparty.cmake)
endif()

if (BUILD_TESTS AND NOT DOWNLOAD_THIRDPARTY)
    add_subdirectory(test)
endif()