# For GoogleTest
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# SM
add_executable(E2E_SM_test E2E_SM_test.cpp)
target_link_libraries(E2E_SM_test PUBLIC e2elib gtest_main)
gtest_discover_tests(E2E_SM_test)

# P01
add_executable(E2E_P01_test E2E_P01_test.cpp)
target_link_libraries(E2E_P01_test PUBLIC e2elib gtest_main)
gtest_discover_tests(E2E_P01_test)
