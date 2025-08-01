set(BIN_SOURCE main.cpp)
set(BIN_HEADERS)

set(TARGET transform_feedback)

link_directories(${PROJECT_SOURCE_DIR}/sgltk/lib)
include_directories(${PROJECT_SOURCE_DIR}/sgltk/include)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(${TARGET} ${BIN_SOURCE} ${BIN_HEADERS})

target_compile_definitions(${TARGET} PRIVATE ${TYPE})

foreach(lib ${LIBS})
	target_link_libraries(${TARGET} ${lib})
endforeach()
