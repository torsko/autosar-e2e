set(TARGET_NAME e2e)

set(SRCS
    src/E2E.c
    src/E2E_SM.c
    src/E2E_P01.c
    src/E2E_P05.c
)


add_library(${TARGET_NAME} ${SRCS})
target_include_directories(${TARGET_NAME} PUBLIC include)
target_link_libraries(${TARGET_NAME} common crc)
target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic -Werror --coverage)
target_link_options(${TARGET_NAME} PUBLIC --coverage)

add_subdirectory(test)
