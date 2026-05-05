# Grassroots Assembler

set(TARGET_NAME gsm)

add_executable(${TARGET_NAME} main.c assembler.c)
target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra)

target_link_libraries(gsm PRIVATE CCORE::std)
