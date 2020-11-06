# - Try to find the GLAD library
# Once done this will define
#
#  GLAD_FOUND - system has GLAD
#  GLAD_INCLUDE_DIR - **the** GLAD include directory
#  GLAD_INCLUDE_DIRS - GLAD include directories
#  GLAD_SOURCES - the GLAD source files

if(NOT GLAD_FOUND)
    find_path(GLAD_INCLUDE_DIR
            NAMES glad/glad.h
            PATHS ${CMAKE_SOURCE_DIR}/external
            DOC "The glad include directory"
            NO_DEFAULT_PATH)

    if(GLAD_INCLUDE_DIR)
        set(GLAD_FOUND TRUE)
        set(GLAD_DIRECTORY ${CMAKE_SOURCE_DIR}/external/glad)
        set(GLAD_INCLUDE_DIRS ${GLAD_INCLUDE_DIR})
        set(GLAD_SOURCE_DIRS ${GLAD_DIRECTORY})

        message("glad found in " GLAD_DIRECTORY = ${GLAD_DIRECTORY})
        message("              " GLAD_INCLUDE_DIRS =  ${GLAD_INCLUDE_DIRS})
        message("              " GLAD_SOURCE_DIRS =  ${GLAD_SOURCE_DIRS})
    else()
        message("+--------------------------------------+")
        message("| GLAD not found, please download GLAD |")
        message("+--------------------------------------+")
        message(FATAL_ERROR "")
    endif()
endif()
