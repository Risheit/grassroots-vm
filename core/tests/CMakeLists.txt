add_executable(test test_runner.c)
target_link_libraries(test PRIVATE std)
target_include_directories(test PRIVATE ${PROJECT_BINARY_DIR} "${PROJECT_SOURCE_DIR}/core")
