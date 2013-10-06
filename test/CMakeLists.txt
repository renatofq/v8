enable_testing()

add_executable(test_map map_test.c)
target_link_libraries(test_map v8)

add_test(test_map_test test_map)
set_tests_properties(test_map_test 
  PROPERTIES
  FAIL_REGULAR_EXPRESSION "fail")
