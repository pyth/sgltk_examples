cmake_minimum_required(VERSION 2.8.4)
project(sgltk_example)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wno-int-to-pointer-cast")
endif()

set(ExternalProjectCMakeArgs
		-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
		-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})

add_subdirectory(library_test)
add_subdirectory(hello_triangle)

if(EXISTS sgltk/CMakeLists.txt)
	add_subdirectory(sgltk)
endif()
