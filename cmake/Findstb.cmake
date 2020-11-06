# - Try to find the STB library
# Once done this will define
#
#  STB_FOUND - system has STB
#  STB_INCLUDE_DIR - **the** STB include directory
#  STB_INCLUDE_DIRS - STB include directories
#  STB_SOURCES - the STB source files

if(NOT STB_FOUND)
	find_path(STB_INCLUDE_DIR
		NAMES stb.h
	   	PATHS ${CMAKE_SOURCE_DIR}/external/stb
		DOC "The STB include directory"
		NO_DEFAULT_PATH)

	if(STB_INCLUDE_DIR)
	   set(STB_FOUND TRUE)
	   set(STB_DIRECTORY ${CMAKE_SOURCE_DIR}/external/stb)
	   set(STB_INCLUDE_DIRS ${STB_DIRECTORY})
	   set(STB_SOURCE_DIRS ${STB_DIRECTORY})

	   message("stb found in " STB_DIRECTORY = ${STB_DIRECTORY})
	   message("             " STB_INCLUDE_DIRS =  ${STB_INCLUDE_DIRS})
	   message("             " STB_SOURCE_DIRS =  ${STB_SOURCE_DIRS})
	else()
	   message("+------------------------------------+")
	   message("| STB not found, please download STB |")
	   message("+------------------------------------+")
	   message(FATAL_ERROR "")
	endif()
endif()
