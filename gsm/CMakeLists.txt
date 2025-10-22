# Grassroots Assembler

set(EXE_NAME gsm)

add_executable(${EXE_NAME} main.c)
target_link_libraries(gsm PRIVATE std)
target_include_directories(gsm PRIVATE ${PROJECT_BINARY_DIR}
                                       "${PROJECT_SOURCE_DIR}/core")

# Apple requires binaries to be signed to access certain folders (ex.
# Documents). This signs the binary locally to allow access.
if(APPLE)
  execute_process(COMMAND codesign --sign - "${PROJECT_BINARY_DIR}/${EXE_NAME}")
endif()
