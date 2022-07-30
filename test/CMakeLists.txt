enable_testing()

file(GLOB TestSource *.c *.cpp)
find_package(GTest REQUIRED)
add_executable(test_unit test_googletest.cpp)
target_link_libraries(
  test_unit
  ${GTEST_LIBRARIES}
  ${GTEST_MAIN_LIBRARIES} intf
)

target_include_directories(test_unit PRIVATE ${GTEST_INCLUDE_DIRS})
target_sources(test_unit PRIVATE ${TestSource})