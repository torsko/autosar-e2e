cmake_minimum_required(VERSION 3.16)
project(autosar)

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
