# - Try to find the NANOFLANN library
# Once done this will define
#
#  NANOFLANN_FOUND - system has NANOFLANN
#  NANOFLANN_INCLUDE_DIR - **the** NANOFLANN include directory
#  NANOFLANN_INCLUDE_DIRS - NANOFLANN include directories
#  NANOFLANN_SOURCES - the NANOFLANN source files

if(NOT NANOFLANN_FOUND)
	find_path(NANOFLANN_INCLUDE_DIR
		NAMES nanoflann.hpp
	   	PATHS ${CMAKE_SOURCE_DIR}/external/nanoflann/include
		DOC "The NANOFLANN include directory"
		NO_DEFAULT_PATH)

	if(NANOFLANN_INCLUDE_DIR)
	   set(NANOFLANN_FOUND TRUE)
	   set(NANOFLANN_DIRECTORY ${CMAKE_SOURCE_DIR}/external/nanoflann)
	   set(NANOFLANN_INCLUDE_DIRS ${NANOFLANN_DIRECTORY}/include/)
	   set(NANOFLANN_SOURCE_DIRS ${NANOFLANN_DIRECTORY}/examples)

	   message("nanoflann found in " NANOFLANN_DIRECTORY = ${NANOFLANN_DIRECTORY})
	   message("                   " NANOFLANN_INCLUDE_DIRS =  ${NANOFLANN_INCLUDE_DIRS})
	   message("                   " NANOFLANN_SOURCE_DIRS =  ${NANOFLANN_SOURCE_DIRS})
	else()
	   message("+------------------------------------------------+")
	   message("| NANOFLANN not found, please download NANOFLANN |")
	   message("+------------------------------------------------+")
	   message(FATAL_ERROR "")
	endif()
endif()
