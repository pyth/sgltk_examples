cmake_minimum_required(VERSION 3.15)
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

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ExternalProjectCMakeArgs
		-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
		-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})

if(WIN32)
	set(LIBS OpenGL32 glew32 sgltk)
else()
	set(LIBS GL GLEW sgltk)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/bin)
endforeach()

find_library(sgltk_FOUND sgltk)
if(sgltk_FOUND)
	message(STATUS "FOUND sgltk")
endif()

find_library(assimp_FOUND assimp)
if(assimp_FOUND)
	set(HAVE_ASSIMP_H 1)
endif()

foreach(target ${TARGETS})
	add_subdirectory(${target})
endforeach()
