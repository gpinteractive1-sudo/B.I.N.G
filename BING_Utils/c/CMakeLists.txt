if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Cecho/src/cecho.c")
    add_executable(cecho Cecho/src/cecho.c)
else()
    add_executable(cecho cecho/src/cecho.c)
endif()
install(TARGETS cecho DESTINATION bin)


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Cfalse/src/cfalse.c")
    add_executable(cfalse Cfalse/src/cfalse.c)
else()
    add_executable(cfalse cfalse/src/cfalse.c)
endif()
install(TARGETS cfalse DESTINATION bin)


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Cing/src/cing.c")
    add_executable(cing Cing/src/cing.c)
else()
    add_executable(cing cing/src/cing.c)
endif()
install(TARGETS cing DESTINATION bin)


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Ctrue/src/ctrue.c")
    add_executable(ctrue Ctrue/src/ctrue.c)
else()
    add_executable(ctrue ctrue/src/ctrue.c)
endif()
install(TARGETS ctrue DESTINATION bin)


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Cpwd/src/cpwd.c")
    add_executable(cpwd Cpwd/src/cpwd.c)
else()
    add_executable(cpwd cpwd/src/cpwd.c)
endif()
install(TARGETS cpwd DESTINATION bin)


if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Cwhoami/src/cwhoami.c")
    add_executable(cwhoami Cwhoami/src/cwhoami.c)
else()
    add_executable(cwhoami cwhoami/src/cwhoami.c)
endif()
install(TARGETS cwhoami DESTINATION bin)



if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Ccommand/src/ccommand.c")
    add_executable(ccommand Ccommand/src/ccommand.c)
else()
    add_executable(ccommand ccommand/src/ccommand.c)
endif()
install(TARGETS ccommand DESTINATION bin)
