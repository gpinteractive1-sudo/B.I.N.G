add_executable(bforth src/bforth.c)
install(TARGETS bforth
    RUNTIME DESTINATION bin
)
