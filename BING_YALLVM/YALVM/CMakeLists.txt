add_library(YALVM STATIC
    src/yalvm.c
    src/yalvm_bc.c
)

target_include_directories(YALVM PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(YALVM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../ZeIRbra/src
)

target_link_libraries(YALVM PUBLIC zeirbra)

