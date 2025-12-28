# Grassroots VM

set(EXE_NAME gvm)

add_executable(${EXE_NAME} main.c)
target_link_libraries(gvm PRIVATE CCORE::std)

# Apple requires binaries to be signed to access certain folders (ex.
# Documents). This signs the binary locally to allow access.
if(APPLE)
  execute_process(COMMAND codesign --sign - "${PROJECT_BINARY_DIR}/${EXE_NAME}")
endif()
