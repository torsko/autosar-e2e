set(TARGET_NAME crc)

set(SRCS
    src/Crc8.c
    src/Crc8H2F.c
    src/Crc16.c
    src/Crc16ARC.c
    src/Crc32.c
    src/Crc32P4.c
    src/Crc64.c
)

add_library(${TARGET_NAME} ${SRCS})
target_include_directories(${TARGET_NAME} PUBLIC include)
target_link_libraries(${TARGET_NAME} common)
target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic -Werror --coverage)
target_link_options(${TARGET_NAME} PUBLIC --coverage)

add_subdirectory(test)
