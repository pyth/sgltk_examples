cmake_minimum_required(VERSION 2.8.4)
project(sgltk_example)

set(TARGETS system_info
	hello_triangle
	loading_models
	instanced_rendering
	controller_handling
	library_test)

set(TYPE DYNAMIC)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-int-to-pointer-cast")
endif()

set(ExternalProjectCMakeArgs
		-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
		-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})

set(LIBS sgltk)
if(WIN32)
	set(LIBS OpenGL32 glew32 ${LIBS})
else()
	set(LIBS GL GLEW ${LIBS})
endif()

find_library(SGLTK_FOUND sgltk)
if(${SGLTK_FOUND})
	message(STATUS "FOUND sgltk")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/bin)
endforeach()

foreach(target ${TARGETS})
	add_subdirectory(${target})
endforeach()

if(EXISTS sgltk/CMakeLists.txt)
	add_subdirectory(sgltk)
endif()
