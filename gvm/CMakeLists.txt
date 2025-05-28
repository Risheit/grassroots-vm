add_executable(gvm main.c)
target_link_libraries(gvm PRIVATE std)
target_include_directories(gvm PRIVATE ${PROJECT_BINARY_DIR} "${PROJECT_SOURCE_DIR}/core")
