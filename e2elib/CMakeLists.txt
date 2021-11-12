set(SRCS
    src/E2E.c
    src/E2E_SM.c
    src/E2E_P01.c
)

add_library(e2elib ${SRCS})
target_include_directories(e2elib PUBLIC include)

add_subdirectory(test)
