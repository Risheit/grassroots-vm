# Grassroots VM

set(TARGET_NAME gvm)

set(SOURCES VirtualMachine.c)

add_executable(${TARGET_NAME} main.c ${SOURCES})
target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra)
target_compile_definitions(${TARGET_NAME} PRIVATE TEST_STATIC=static)

target_link_libraries(gvm PRIVATE CCORE::std)

add_subdirectory(test)
