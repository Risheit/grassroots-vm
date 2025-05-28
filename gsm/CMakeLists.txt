# Grassroots Assembler

add_executable(gsm main.c)
target_link_libraries(gsm PRIVATE std)
target_include_directories(gsm PRIVATE ${PROJECT_BINARY_DIR} "${PROJECT_SOURCE_DIR}/core")
