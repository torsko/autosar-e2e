cmake_minimum_required(VERSION 3.16)
project(autosar)

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

    #get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    foreach(dir ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
      message(STATUS "dir='${dir}'")
    endforeach()

    add_custom_target(cppcheck
        COMMAND ${CMAKE_C_CPPCHECK} --enable=all -i test ${PROJECT_SOURCE_DIR}/e2elib ${PROJECT_SOURCE_DIR}/crclib ${PROJECT_SOURCE_DIR}/common
                                    -I ${PROJECT_SOURCE_DIR}/e2elib/include
                                    -I ${PROJECT_SOURCE_DIR}/crclib/include
                                    -I ${PROJECT_SOURCE_DIR}/common/include
        COMMENT "Running cppcheck with all warnings!"
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

add_subdirectory(common)
add_subdirectory(crclib)
add_subdirectory(e2elib)
