set(BIN_SOURCE main.cpp)
set(BIN_HEADERS)

set(TARGET controller_handling)

add_executable(${TARGET} ${BIN_SOURCE} ${BIN_HEADERS})
target_link_libraries(${TARGET} PRIVATE sgltk::sgltk)

target_compile_definitions(${TARGET} PRIVATE ${TYPE})