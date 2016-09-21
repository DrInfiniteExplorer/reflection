
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
  target_link_libraries (${TEST_TARGET} LINK_PUBLIC Reflection gtest_main gmock diaguids.lib)
  if(WIN32)
    set_target_properties(${TEST_TARGET} PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
#	add_dependencies(${TEST_TARGET} ReflectionTestData)
	add_custom_command(TARGET ${TEST_TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_PDB_FILE:ReflectionTestData>" ${CMAKE_CURRENT_BINARY_DIR})
	
  endif()
  add_test(${TEST_TARGET} ${TEST_TARGET})
endfunction()

add_subdirectory(test_data)

if(WIN32)
	
add_google_test("load_pdb.cpp")
else()

endif()
