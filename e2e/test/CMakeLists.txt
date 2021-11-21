# SM
add_executable(E2E_SM_tests
    SM/E2E_SM_test.cpp
)
target_link_libraries(E2E_SM_tests PUBLIC e2e gtest_main)
gtest_discover_tests(E2E_SM_tests)

# P01
add_executable(E2E_P01_tests
    P01/E2E_P01CheckInit_tests.cpp
    P01/E2E_P01MapStatusToSM_tests.cpp
    P01/E2E_P01Protect_tests.cpp
    P01/E2E_P01Protect_InputValidation_tests.cpp
    P01/E2E_P01ProtectInit_tests.cpp
)
target_link_libraries(E2E_P01_tests PUBLIC e2e gtest_main)
gtest_discover_tests(E2E_P01_tests)
