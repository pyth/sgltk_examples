set(BIN_SOURCE main.cpp)
set(BIN_HEADERS)

set(TARGET controller_handling)

add_executable(${TARGET} ${BIN_SOURCE} ${BIN_HEADERS})
target_link_libraries(${TARGET} PRIVATE
	sgltk::sgltk
	GLEW::GLEW
	glm::glm
	SDL2::SDL2
	SDL2_image::SDL2_image
	sdl_ttf::sdl_ttf
	assimp::assimp
	GL
)

target_compile_definitions(${TARGET} PRIVATE ${TYPE})