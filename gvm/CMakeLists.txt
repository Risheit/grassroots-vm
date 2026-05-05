# Grassroots VM

set(EXE_NAME gvm)

add_executable(${EXE_NAME} main.c)
target_link_libraries(gvm PRIVATE CCORE::std)

