# - Try to find the TBB library
# Once done this will define
#
#  TBB_FOUND - system has TBB
#  TBB_INCLUDE_DIR - **the** TBB include directory
#  TBB_INCLUDE_DIRS - TBB include directories
#  TBB_SOURCES - the TBB source files

if(NOT TBB_FOUND)
	find_path(TBB_INCLUDE_DIR
		NAMES tbb/tbb.h
	   	PATHS ${CMAKE_SOURCE_DIR}/external/tbb/include
		DOC "The TBB include directory"
		NO_DEFAULT_PATH)

	if(TBB_INCLUDE_DIR)
	   set(TBB_FOUND TRUE)
	   set(TBB_DIRECTORY ${CMAKE_SOURCE_DIR}/external/tbb)
	   set(TBB_INCLUDE_DIRS ${TBB_DIRECTORY}/include)
	   set(TBB_SOURCE_DIRS ${TBB_DIRECTORY}/src)

	   message("tbb found in " TBB_DIRECTORY = ${TBB_DIRECTORY})
	   message("             " TBB_INCLUDE_DIRS =  ${TBB_INCLUDE_DIRS})
	   message("             " TBB_SOURCE_DIRS =  ${TBB_SOURCE_DIRS})
	else()
	   message("+------------------------------------+")
	   message("| TBB not found, please download TBB |")
	   message("+------------------------------------+")
	   message(FATAL_ERROR "")
	endif()
endif()
