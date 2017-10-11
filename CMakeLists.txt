cmake_minimum_required(VERSION 2.8.4)
project(sgltk_example)

set(TARGETS system_info
	hello_triangle
	cubemap
	sphere
	island
	compute_test
	mandelbrot_set
	particle_system
	shadow_mapping
	shadow_mapping2
	loading_models
	transform_feedback
	instanced_rendering
	instanced_rendering2
	controller_handling
	library_test)

set(LIBRARY_PATH "" CACHE PATH "3rd party libraries")
set(INCLUDE_PATH "" CACHE PATH "3rd party library headers")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${LIBRARY_PATH};${INCLUDE_PATH}")

include_directories(${INCLUDE_PATH})
link_directories(${LIBRARY_PATH})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(ExternalProjectCMakeArgs
		-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
		-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})

set(LIBS sgltk)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/bin)
endforeach()

if(EXISTS ${PROJECT_SOURCE_DIR}/sgltk/CMakeLists.txt)
	set(SGLTK_FOUND 1)
	message(STATUS "FOUND sgltk")
	add_subdirectory(sgltk)
else()
	find_library(SGLTK_FOUND sgltk)
	if(${SGLTK_FOUND})
		message(STATUS "FOUND sgltk")
	endif()

	find_library(assimp_FOUND assimp)
	if(assimp_FOUND)
		set(HAVE_ASSIMP_H 1)
	endif()
endif()

foreach(target ${TARGETS})
	add_subdirectory(${target})
endforeach()
