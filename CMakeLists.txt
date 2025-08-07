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

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

find_package(sgltk REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(assimp REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/bin)
endforeach()

foreach(target ${TARGETS})
	add_subdirectory(${target})
endforeach()
