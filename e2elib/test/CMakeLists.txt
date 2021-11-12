# For GoogleTest
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  #URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# P01
add_executable(E2E_P01_test E2E_P01_test.cpp)
target_link_libraries(E2E_P01_test PUBLIC e2elib gtest_main)

add_test(NAME E2E_P01 COMMAND E2E_P01_test)

include(GoogleTest)
#gtest_add_tests(TARGET E2E_P01_test)
