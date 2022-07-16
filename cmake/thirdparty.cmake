message("download thirdparty")

# 下载gtest github经常断 所以使用gitee的开源库了
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://gitee.com/zhang1021/googletest.git
  GIT_TAG        release-1.10.0 # release-1.10.0
)
FetchContent_MakeAvailable(googletest)

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