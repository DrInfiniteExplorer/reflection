cmake_minimum_required (VERSION 2.6)
project (Reflection)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

#add_definitions(-D_NO_CVCONST_H) 
 
include_directories ("${PROJECT_SOURCE_DIR}/include")

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/include/*.h)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

install(TARGETS ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})
install(FILES ${PROJECT_NAME} DESTINATION include/${PROJECT_NAME})

include(CTest)
add_subdirectory(test)

