cmake_minimum_required(VERSION 3.16)
project(autosar)

# Enable cppcheck if
find_program(CMAKE_C_CPPCHECK NAMES cppcheck)
if (CMAKE_C_CPPCHECK)
    list(
        APPEND CMAKE_C_CPPCHECK
            "--enable=style"
            "--inconclusive"
            "--force"
            "--inline-suppr"
            "--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt"
            "--std=c99"
    )

    add_custom_target(cppcheck
        COMMAND ${CMAKE_C_CPPCHECK} --enable=all -i test
            ${PROJECT_SOURCE_DIR}/e2e
            ${PROJECT_SOURCE_DIR}/crc
            ${PROJECT_SOURCE_DIR}/common
            -I ${PROJECT_SOURCE_DIR}/e2e/include
            -I ${PROJECT_SOURCE_DIR}/crc/include
            -I ${PROJECT_SOURCE_DIR}/common/include
        COMMENT "Running cppcheck with all warnings!"
        )

endif()

# Enable generating doxygen documentation with the 'docs' target
find_program(DOXYGEN_BIN doxygen)
if (DOXYGEN_BIN)
  message(STATUS "Doxygen found, adding custom target 'docs' and 'docs_test'")
  add_custom_target(docs
      COMMAND doxygen Doxyfile
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

    add_custom_target(docs_test
      COMMAND doxygen Doxyfile.test
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()

# Enable generating a coverage report if gcovr is installed
find_program(GCOVR_BIN gcovr)
if (GCOVR_BIN)
  add_custom_target(coverage
    COMMAND make && ctest
    COMMAND gcovr -r ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR}
  )
  add_custom_target(coverage_html
    COMMAND make && ctest
    COMMAND mkdir -p coverage
    COMMAND gcovr -r ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} --html --html-details -o coverage/coverage.html
  )

endif()

# Enable testing for the project
enable_testing()

# For GoogleTest
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_compile_options(-ggdb -O0)

add_subdirectory(common)
add_subdirectory(crc)
add_subdirectory(e2e)
