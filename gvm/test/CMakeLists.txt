file(GLOB TESTS CONFIGURE_DEPENDS "*.c")

list(TRANSFORM SOURCES PREPEND "../" OUTPUT_VARIABLE SOURCES_PREFIXED)

set(PREBUILT_SRC_DIR "${CMAKE_SOURCE_DIR}/ext/prebuilt")
set(PREBUILT_OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/prebuilt")

foreach(test ${TESTS})
  get_filename_component(TARGET_NAME ${test} NAME_WE)

  add_executable(${TARGET_NAME} ${test} ${SOURCES_PREFIXED})

  target_include_directories(${TARGET_NAME} PRIVATE ..)
  target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra)
  target_compile_definitions(
    ${TARGET_NAME} PRIVATE TEST_BUILD PREBUILT_DIR="${PREBUILT_OUT_DIR}"
                           TEST_STATIC=)
  target_link_libraries(${TARGET_NAME} PRIVATE CCORE::std)
endforeach()

add_custom_target(
  copy_resources ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${PREBUILT_SRC_DIR}
          ${PREBUILT_OUT_DIR}
  COMMENT "Copying prebuilt files over")
