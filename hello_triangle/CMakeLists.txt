set(BIN_SOURCE main.cpp)
set(BIN_HEADERS)

set(TARGET hello_triangle)

add_executable(${TARGET} ${BIN_SOURCE} ${BIN_HEADERS})
target_link_libraries(${TARGET} PRIVATE
	sgltk::sgltk
	GLEW::GLEW
	glm::glm
	SDL2::SDL2
	SDL2_image::SDL2_image
	sdl_ttf::sdl_ttf
	assimp::assimp
	opengl::opengl
)

if(WIN32)
	if(MSVC)
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:\"mainCRTStartup\"")
	elseif(MINGW)
		set_target_properties(${TARGET} PROPERTIES LINK_FLAGS "-mwindows")
	endif()
endif()

target_compile_definitions(${TARGET} PRIVATE ${TYPE})