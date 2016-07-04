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

add_dependencies(${TARGET} sgltk)

target_link_libraries(${TARGET} sgltk)
target_link_libraries(${TARGET} GL)
target_link_libraries(${TARGET} GLEW)
