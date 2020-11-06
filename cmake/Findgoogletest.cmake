# - Try to find the GOOGLETEST library
# Once done this will define
#
#  GOOGLETEST_FOUND - system has GOOGLETEST
#  GOOGLETEST_INCLUDE_DIR - **the** GOOGLETEST include directory
#  GOOGLETEST_INCLUDE_DIRS - GOOGLETEST include directories
#  GOOGLETEST_SOURCES - the GOOGLETEST source files

if(NOT GOOGLETEST_FOUND)
	find_path(GOOGLETEST_INCLUDE_DIR
		NAMES gtest.h
	   	PATHS ${CMAKE_SOURCE_DIR}/external/googletest/googletest/include/gtest
		DOC "The GOOGLETEST include directory"
		NO_DEFAULT_PATH)

	if(GOOGLETEST_INCLUDE_DIR)
	   set(GOOGLETEST_FOUND TRUE)
	   set(GOOGLETEST_DIRECTORY ${CMAKE_SOURCE_DIR}/external/googletest)
	   set(GOOGLETEST_INCLUDE_DIRS ${GOOGLETEST_DIRECTORY}/googletest/include ${GOOGLETEST_DIRECTORY}/googlemock/include)
	   set(GOOGLETEST_SOURCE_DIRS ${GOOGLETEST_DIRECTORY}/googletest/src ${GOOGLETEST_DIRECTORY}/googlemock/src)

	   message("stb found in " GOOGLETEST_DIRECTORY = ${GOOGLETEST_DIRECTORY})
	   message("             " GOOGLETEST_INCLUDE_DIRS =  ${GOOGLETEST_INCLUDE_DIRS})
	   message("             " GOOGLETEST_SOURCE_DIRS =  ${GOOGLETEST_SOURCE_DIRS})
	else()
	   message("+--------------------------------------------------+")
	   message("| GOOGLETEST not found, please download GOOGLETEST |")
	   message("+--------------------------------------------------+")
	   message(FATAL_ERROR "")
	endif()
endif()
