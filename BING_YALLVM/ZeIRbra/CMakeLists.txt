add_library(zeirbra STATIC
    src/zbr_core.c
    src/zbr_dump.c
)

target_include_directories(zeirbra PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
