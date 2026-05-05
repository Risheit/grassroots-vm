# Grassroots Assembler

set(EXE_NAME gsm)

add_executable(${EXE_NAME} main.c assembler.c)
target_link_libraries(gsm PRIVATE CCORE::std)
