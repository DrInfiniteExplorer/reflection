
set(CMAKE_SRC_ROOT https://github.com/google/googletest.git CACHE STRING "Root of gtest repo to use")

set(BUILD_GTEST ON)
if(WIN32)
	set(gtest_disable_pthreads ON)
endif()

set(gtest_force_shared_crt ON CACHE BOOL "Force google test to behave")

string(FIND ${CMAKE_SRC_ROOT} "http" pos1)
if(${pos1} GREATER "-1")

	if(NOT IS_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gtest)
		message("Will clone from git url")
		execute_process(COMMAND git clone ${CMAKE_SRC_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/gtest --depth=1)

		include(CheckCXXSourceCompiles)
		unset(IS_BAD_MINGW CACHE)
		CHECK_CXX_SOURCE_COMPILES(
			"#include <windows.h>
			_RTL_CRITICAL_SECTION cs;
			int main(int argc, char* argv[]){}"
		IS_BAD_MINGW)
		if(IS_BAD_MINGW)
			message("Will patch gtest's bad assumption about mingw")
			execute_process(COMMAND git -C ${CMAKE_CURRENT_BINARY_DIR}/gtest apply ${CMAKE_CURRENT_SOURCE_DIR}/gtest_fix/fix.diff)
		endif()
	endif()
	add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/gtest ${CMAKE_CURRENT_BINARY_DIR}/gtest_build)
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/gtest/googletest/include)
else()
	add_subdirectory(${CMAKE_SRC_ROOT})
	include_directories(${CMAKE_SRC_ROOT}/googletest/include)
endif()

include_directories(../include)


function(add_google_test TEST_SOURCE)
  get_filename_component(TEST_TARGET ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_TARGET} ${TEST_SOURCE})
  target_link_libraries (${TEST_TARGET} LINK_PUBLIC gtest_main gmock)
  if(WIN32)
    set_target_properties(${TEST_TARGET} PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
  endif()
  add_test(${TEST_TARGET} ${TEST_TARGET})
endfunction()

function(add_named_test TEST_NAME TEST_SOURCE)
  get_filename_component(TEST_TARGET ${TEST_SOURCE} NAME_WE)
  add_executable(${TEST_TARGET} ${TEST_SOURCE} testinterface.cpp)
  target_link_libraries(${TEST_TARGET} ${PROJECT_NAME})
  if(WIN32)
    set_target_properties(${TEST_TARGET} PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
  endif()
endfunction()

function(expect TEST_EXE_NAME TEST_NAME OUTPUT)
  add_test(${TEST_NAME} ${TEST_EXE_NAME})
  set_tests_properties (${TEST_NAME}
    PROPERTIES PASS_REGULAR_EXPRESSION ${OUTPUT}
	)	
endfunction()

add_named_test("main" main.cpp)
#expect("iterator.foreach" "^0 1 2 3 4 5 6 \n$")

expect("main" "Test1" "\nin TestClass::funcA, m_member=2\n")
expect("main" "Test2" "\nin TestClass::funcB, m_member=2\n")
expect("main" "Test3" "\nin TestClass::funcA, m_member=3\n")
expect("main" "Test4" "\nin TestClass::funcB, m_member=3\n")
expect("main" "Test5" "\nin TestClass::funcA, m_member=4\n")
expect("main" "Test6" "\nin TestClass::funcB, m_member=4\n")
expect("main" "Test7" "\nin TestClass::funcA, m_member=9\n")
expect("main" "Test8" "\nin TestClass::funcB, m_member=9\n")
expect("main" "Test9" "\nin TestClass::funcA, m_member=5\n")
expect("main" "Test10" "\nin TestClass::funcB, m_member=5\n")

add_named_test("test_function" test_function.cpp)
expect("test_function" "test.func1" "\npointer to int\\(argument which is char, argument which is int, argument which is unsigned int, argument which is pointer to void, argument which is pointer to wchar_t, \\)\n")
expect("test_function" "test.func2" "\nException: Invalid function types! Cant call a \"void\\(\\)\" as if it where a \"pointer to int\\(argument which is char, argument which is int, argument which is unsigned int, argument which is pointer to void, argument which is pointer to wchar_t, \\)\"\n")

