# - Try to find the GLFW3 library
# Once done this will define
#
#  GLFW3_FOUND - system has GLFW3
#  GLFW3_INCLUDE_DIR - **the** GLFW3 include directory
#  GLFW3_INCLUDE_DIRS - GLFW3 include directories
#  GLFW3_SOURCES - the GLFW3 source files

if(NOT GLFW3_FOUND)
	find_path(GLFW3_INCLUDE_DIR
			NAMES GLFW/glfw3.h
			PATHS ${CMAKE_SOURCE_DIR}/external/glfw/include
			DOC "The glfw include directory"
			NO_DEFAULT_PATH)

	if(GLFW3_INCLUDE_DIR)
		set(GLFW3_FOUND TRUE)
		set(GLFW3_DIRECTORY ${CMAKE_SOURCE_DIR}/external/glfw)
		set(GLFW3_INCLUDE_DIRS ${GLFW3_DIRECTORY}/include)
		set(GLFW3_SOURCES_DIRS ${GLFW3_DIRECTORY}/src)

		message("glfw found in " GLFW3_DIRECTORY = ${GLFW3_DIRECTORY})
		message("              " GLFW3_INCLUDE_DIRS =  ${GLFW3_INCLUDE_DIRS})
		message("              " GLFW3_SOURCES_DIRS =  ${GLFW3_SOURCES_DIRS})
	else()
		message("+----------------------------------------+")
		message("| GLFW3 not found, please download GLFW3 |")
		message("+----------------------------------------+")
		message(FATAL_ERROR "")
	endif()
endif()
