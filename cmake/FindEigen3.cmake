# - Try to find the EIGEN3 library
# Once done this will define
#
#  EIGEN3_FOUND - system has EIGEN3
#  EIGEN3_INCLUDE_DIR - **the** EIGEN3 include directory
#  EIGEN3_INCLUDE_DIRS - EIGEN3 include directories
#  EIGEN3_SOURCES - the EIGEN3 source files

if(NOT EIGEN3_FOUND)
    find_path(EIGEN3_INCLUDE_DIR
            NAMES Eigen/Dense
            PATHS ${CMAKE_SOURCE_DIR}/external/eigen/
            DOC "The eigen include directory"
            NO_DEFAULT_PATH)

    if(EIGEN3_INCLUDE_DIR)
        set(EIGEN3_FOUND TRUE)
        set(EIGEN3_DIRECTORY ${CMAKE_SOURCE_DIR}/external/eigen)
        set(EIGEN3_INCLUDE_DIRS ${EIGEN3_DIRECTORY})
        set(EIGEN3_SOURCES_DIRS ${EIGEN3_DIRECTORY}/Eigen/src)

        message("eigen found in " EIGEN3_DIRECTORY = ${EIGEN3_DIRECTORY})
        message("               " EIGEN3_INCLUDE_DIRS =  ${EIGEN3_INCLUDE_DIRS})
        message("               " EIGEN3_SOURCES_DIRS =  ${EIGEN3_SOURCES_DIRS})
    else()
        message("+------------------------------------------+")
        message("| EIGEN3 not found, please download EIGEN3 |")
        message("+------------------------------------------+")
        message(FATAL_ERROR "")
    endif()
endif()
