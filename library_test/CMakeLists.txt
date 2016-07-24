set(BIN_SOURCE main.cpp gui.cpp)
set(BIN_HEADERS gui.h)

set(TARGET library_test)

link_directories(${PROJECT_SOURCE_DIR}/sgltk/lib)
include_directories(${PROJECT_SOURCE_DIR}/sgltk/include)

find_library(SGLTK_FOUND sgltk)
if(${SGLTK_FOUND})
	message(STATUS "FOUND sgltk")
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(${TARGET} ${BIN_SOURCE} ${BIN_HEADERS})

target_compile_definitions(${TARGET} PRIVATE ${TYPE})

if(EXISTS ${PROJECT_SOURCE_DIR}/sgltk/CMakeLists.txt)
	add_dependencies(${TARGET} sgltk)
	if(WIN32)
		add_custom_command(TARGET ${TARGET} PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
			${PROJECT_BINARY_DIR}/sgltk/lib/sgltk.dll
			${PROJECT_BINARY_DIR}/bin)
	endif()
endif()

foreach(lib ${LIBS})
	target_link_libraries(${TARGET} ${lib})
endforeach()
