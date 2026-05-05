# Grassroots VM

set(TARGET_NAME gvm)

set(SOURCES vm_passes.c)

add_executable(${TARGET_NAME} main.c ${SOURCES})
target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra)

target_link_libraries(gvm PRIVATE CCORE::std)

add_subdirectory(test)
