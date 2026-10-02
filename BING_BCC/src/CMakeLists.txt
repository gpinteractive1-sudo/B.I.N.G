add_executable(bbc b/bbc.c)


install(TARGETS bbc


    RUNTIME DESTINATION bin


)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
    DESTINATION   bin
  )
