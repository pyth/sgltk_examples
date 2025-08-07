if(NOT assimp_FOUND)
	return()
endif()

set(BIN_SOURCE main.cpp)
set(BIN_HEADERS)

set(TARGET loading_models)

add_executable(${TARGET} ${BIN_SOURCE} ${BIN_HEADERS})
target_link_libraries(${TARGET} PRIVATE sgltk::sgltk)

if(WIN32)
	if(MSVC)
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
	elseif(MINGW)
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-mwindows")
	endif()
endif()

target_compile_definitions(${TARGET} PRIVATE ${TYPE})