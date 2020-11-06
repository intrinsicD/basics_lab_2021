# - Try to find the RPLY library
# Once done this will define
#
#  RPLY_FOUND - system has RPLY
#  RPLY_INCLUDE_DIR - **the** RPLY include directory
#  RPLY_INCLUDE_DIRS - RPLY include directories
#  RPLY_SOURCES - the RPLY source files

if(NOT RPLY_FOUND)
    find_path(RPLY_INCLUDE_DIR
            NAMES rply/rply.h
            PATHS ${CMAKE_SOURCE_DIR}/external
            DOC "The rply include directory"
            NO_DEFAULT_PATH)

    if(RPLY_INCLUDE_DIR)
        set(RPLY_FOUND TRUE)
        set(RPLY_DIRECTORY ${CMAKE_SOURCE_DIR}/external/rply)
        set(RPLY_INCLUDE_DIRS ${RPLY_INCLUDE_DIR})
        set(RPLY_SOURCE_DIRS ${RPLY_DIRECTORY})

        message("rply found in " RPLY_DIRECTORY = ${RPLY_DIRECTORY})
        message("              " RPLY_INCLUDE_DIRS =  ${RPLY_INCLUDE_DIRS})
        message("              " RPLY_SOURCE_DIRS =  ${RPLY_SOURCE_DIRS})
    else()
        message("+--------------------------------------+")
        message("| RPLY not found, please download RPLY |")
        message("+--------------------------------------+")
        message(FATAL_ERROR "")
    endif()
endif()
