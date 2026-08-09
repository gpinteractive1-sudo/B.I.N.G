cmake_minimum_required(VERSION 3.10)
project(BING_Core_Utils C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic -O2)
include(GNUInstallDirs)
include(CheckFunctionExists)
check_function_exists(getgrouplist HAVE_GETGROUPLIST)
if(HAVE_GETGROUPLIST)
    add_compile_definitions(HAVE_GETGROUPLIST=1)
endif()
function(add_c_tool TOOLNAME)
    string(LENGTH "${TOOLNAME}" TOOL_LEN)
    if(TOOL_LEN EQUAL 0)
        message(WARNING "add_c_tool called with empty TOOLNAME")
        return()
    endif()
    string(SUBSTRING "${TOOLNAME}" 0 1 FIRST_CHAR)
    math(EXPR REST_LEN "${TOOL_LEN} - 1")
    if(REST_LEN GREATER 0)
        string(SUBSTRING "${TOOLNAME}" 1 ${REST_LEN} REST)
    else()
        set(REST "")
    endif()
    string(TOUPPER "${FIRST_CHAR}" FIRST_UP)
    set(DIR_CAP "${FIRST_UP}${REST}")
    set(SRC_LOWER "${CMAKE_CURRENT_SOURCE_DIR}/${TOOLNAME}/src/${TOOLNAME}.c")
    set(SRC_CAP "${CMAKE_CURRENT_SOURCE_DIR}/${DIR_CAP}/src/${TOOLNAME}.c")
    if(EXISTS "${SRC_CAP}")
        add_executable(${TOOLNAME} "${SRC_CAP}")
    elseif(EXISTS "${SRC_LOWER}")
        add_executable(${TOOLNAME} "${SRC_LOWER}")
    else()
        message(WARNING "Source for '${TOOLNAME}' not found (checked: ${SRC_CAP} , ${SRC_LOWER}). Skipping ${TOOLNAME}.")
        return()
    endif()
    install(TARGETS ${TOOLNAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()
set(BING_UTILS
    cecho
    cfalse
    cing
    ctrue
    cpwd
    ccommand
    cid
    cmkdir
    csleep
)
foreach(tool ${BING_UTILS})
    add_c_tool(${tool})
endforeach()
