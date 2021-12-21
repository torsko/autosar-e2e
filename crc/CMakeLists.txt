set(SMALL_TARGET crc_small)
set(SMALL_SRCS
    src/small/Crc8.c
    src/small/Crc8H2F.c
    src/small/Crc16.c
    src/small/Crc16ARC.c
    src/small/Crc32.c
    src/small/Crc32P4.c
    src/small/Crc64.c
)

add_library(${SMALL_TARGET} ${SMALL_SRCS})
target_include_directories(${SMALL_TARGET} PUBLIC include)
target_link_libraries(${SMALL_TARGET} common)
target_compile_options(${SMALL_TARGET} PRIVATE -Wall -Wextra -pedantic -Werror --coverage)
target_link_options(${SMALL_TARGET} PUBLIC --coverage)


set(FAST_TARGET crc_fast)
set(FAST_SRCS
    src/fast/Crc8.c
    src/fast/Crc8H2F.c
    src/fast/Crc16.c
    src/fast/Crc16ARC.c
    src/fast/Crc32.c
    src/fast/Crc32P4.c
    src/fast/Crc64.c
)

add_library(${FAST_TARGET} ${FAST_SRCS})
target_include_directories(${FAST_TARGET} PUBLIC include)
target_link_libraries(${FAST_TARGET} common)
target_compile_options(${FAST_TARGET} PRIVATE -Wall -Wextra -pedantic -Werror --coverage)
target_link_options(${FAST_TARGET} PUBLIC --coverage)

add_subdirectory(test)
