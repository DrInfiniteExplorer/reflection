
include_directories(../include)

function(add_named_test TEST_NAME TEST_SOURCE)
  get_filename_component(TEST_TARGET ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_TARGET} ${TEST_SOURCE} testinterface.cpp)
  target_link_libraries(${TEST_TARGET} ${PROJECT_NAME})
  if(WIN32)
    set_target_properties(${TEST_TARGET} PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
  endif()
  add_test(${TEST_NAME} ${TEST_TARGET})
endfunction()

function(expect TEST_NAME OUTPUT)
  set_tests_properties (${TEST_NAME}
    PROPERTIES PASS_REGULAR_EXPRESSION ${OUTPUT}
	)	
endfunction()

add_named_test("main" main.cpp)
#expect("iterator.foreach" "^0 1 2 3 4 5 6 \n$")

expect("main" "\nin TestClass::funcA, m_member=2\n")
expect("main" "\nin TestClass::funcB, m_member=2\n")
expect("main" "\nin TestClass::funcA, m_member=3\n")
expect("main" "\nin TestClass::funcB, m_member=3\n")
expect("main" "\nin TestClass::funcA, m_member=4\n")
expect("main" "\nin TestClass::funcB, m_member=4\n")
expect("main" "\nin TestClass::funcA, m_member=9\n")
expect("main" "\nin TestClass::funcB, m_member=9\n")
expect("main" "\nin TestClass::funcA, m_member=5\n")
expect("main" "\nin TestClass::funcB, m_member=5\n")

add_named_test("test.function" test_function.cpp)
expect("test.function" "\npointer to int\\(argument which is char, argument which is int, argument which is unsigned int, argument which is pointer to void, argument which is pointer to wchar_t, \\)\n")

