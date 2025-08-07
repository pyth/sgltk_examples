set(BIN_SOURCE main.cpp)
set(BIN_HEADERS)

set(TARGET compute_test)

add_executable(${TARGET} ${BIN_SOURCE} ${BIN_HEADERS})

target_compile_definitions(${TARGET} PRIVATE ${TYPE})
target_link_libraries(${TARGET} PRIVATE sgltk::sgltk)
