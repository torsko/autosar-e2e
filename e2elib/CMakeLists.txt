set(SRCS
    src/E2E.c
    src/E2E_SM.c
    src/E2E_P01.c
)

add_library(e2elib ${SRCS})
target_include_directories(e2elib PUBLIC include)
target_link_libraries(e2elib common crclib)

add_subdirectory(test)
