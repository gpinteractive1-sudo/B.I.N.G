add_executable(bbc b/bbc.c)


install(TARGETS bbc


    RUNTIME DESTINATION bin


)

install(DIRECTORY b/include
    DESTINATION   bin
  )
