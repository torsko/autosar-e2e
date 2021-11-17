set(SRCS
    src/Crc8.c
)

add_library(crclib ${SRCS})
target_include_directories(crclib PUBLIC include)
target_link_libraries(crclib common)

add_subdirectory(test)
