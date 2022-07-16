enable_testing()

file(GLOB TestSource *.c *.cpp)
add_executable(test_googletest test_googletest.cpp)
target_link_libraries(
  test_googletest
  gtest_main intf
)
target_sources(test_googletest PRIVATE ${TestSource})