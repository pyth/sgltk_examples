if(NOT assimp_FOUND)
	return()
endif()

set(BIN_SOURCE main.cpp)
set(BIN_HEADERS)

set(TARGET shadow_mapping2)

link_directories(${PROJECT_SOURCE_DIR}/sgltk/lib)
include_directories(${PROJECT_SOURCE_DIR}/sgltk/include)

find_library(SGLTK_FOUND sgltk)
if(${SGLTK_FOUND})
	message(STATUS "FOUND sgltk")
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(${TARGET} ${BIN_SOURCE} ${BIN_HEADERS})

if(WIN32)
	if(MSVC)
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
	elseif(MINGW)
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-mwindows")
	endif()
endif()

target_compile_definitions(${TARGET} PRIVATE ${TYPE})

if(EXISTS ${PROJECT_SOURCE_DIR}/sgltk/CMakeLists.txt)
	add_dependencies(${TARGET} sgltk)
	if(WIN32)
		add_custom_command(TARGET ${TARGET} PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${PROJECT_BINARY_DIR}/sgltk/lib/sgltk.dll
			${PROJECT_BINARY_DIR}/bin)
	endif()
endif()

foreach(lib ${LIBS})
	target_link_libraries(${TARGET} ${lib})
endforeach()
