add_executable(bbc src/b/bbc.c)


install(TARGETS bbc


    RUNTIME DESTINATION bin


)

install(DIRECTORY src/b/include/macro
    DESTINATION   bin/include
  )
