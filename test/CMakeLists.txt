
include_directories(../include)

function(add_named_test TEST_NAME TEST_SOURCE)
  get_filename_component(TEST_TARGET ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_TARGET} ${TEST_SOURCE} testinterface.cpp)
  target_link_libraries(${TEST_TARGET} ${PROJECT_NAME})

  add_test(${TEST_NAME} ${TEST_TARGET})
endfunction()

function(expect TEST_NAME OUTPUT)
  set_tests_properties (${TEST_NAME}
    PROPERTIES PASS_REGULAR_EXPRESSION ${OUTPUT}
	)	
endfunction()

add_named_test("main" main.cpp)
#expect("iterator.foreach" "^0 1 2 3 4 5 6 \n$")

