message("download thirdparty ${CMAKE_MAKE_PROGRAM}")

# # 下载gtest github经常断 所以使用gitee的开源库了
# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   GIT_REPOSITORY https://gitee.com/zhang1021/googletest.git
#   GIT_TAG        release-1.10.0 # release-1.10.0
# )
# FetchContent_MakeAvailable(googletest)

# include(ExternalProject)
# ExternalProject_Add(googletest
#     GIT_REPOSITORY https://github.com/google/googletest.git
#     GIT_TAG master
#     SOURCE_DIR "${CMAKE_BINARY_DIR}/googletest-src"
#     BINARY_DIR "${CMAKE_BINARY_DIR}/googletest-build"
#     CONFIGURE_COMMAND ""
#     BUILD_COMMAND ""
#     INSTALL_COMMAND ""
#     TEST_COMMAND ""
# )

# 类似https://blog.csdn.net/zym326975/article/details/106118063
include(ExternalProject)
ExternalProject_Add(gtest
  INSTALL_DIR ${CMAKE_SOURCE_DIR}/third
  GIT_REPOSITORY https://gitee.com/zhang1021/googletest.git
  GIT_TAG        release-1.10.0 # release-1.10.0
  PREFIX ${CMAKE_SOURCE_DIR}/source/third
  CONFIGURE_COMMAND ""
  CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
              -DCMAKE_BUILD_TYPE=Release
              -DCMAKE_MAKE_PROGRAM={CMAKE_MAKE_PROGRAM}
              -DCMAKE_C_COMPILE={CMAKE_C_COMPILE}
              -DCMAKE_CXX_COMPILE={CMAKE_CXX_COMPILE}
              -DCMAKE_VERBOSE_MAKEFILE={CMAKE_VERBOSE_MAKEFILE}
  # INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
  )