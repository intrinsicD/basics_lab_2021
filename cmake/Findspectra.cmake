# - Try to find the SPECTRA library
# Once done this will define
#
#  SPECTRA_FOUND - system has SPECTRA
#  SPECTRA_INCLUDE_DIR - **the** SPECTRA include directory
#  SPECTRA_INCLUDE_DIRS - SPECTRA include directories
#  SPECTRA_SOURCES - the SPECTRA source files

if(NOT SPECTRA_FOUND)
	find_path(SPECTRA_INCLUDE_DIR
		NAMES Spectra/GenEigsBase.h
	   	PATHS ${CMAKE_SOURCE_DIR}/external/spectra/include/
		DOC "The SPECTRA include directory"
		NO_DEFAULT_PATH)

	if(SPECTRA_INCLUDE_DIR)
	   set(SPECTRA_FOUND TRUE)
	   set(SPECTRA_DIRECTORY ${CMAKE_SOURCE_DIR}/external/spectra)
	   set(SPECTRA_INCLUDE_DIRS ${SPECTRA_DIRECTORY}/include/)
	   set(SPECTRA_SOURCE_DIRS ${SPECTRA_DIRECTORY}/src)

	   message("spectra found in " SPECTRA_DIRECTORY = ${SPECTRA_DIRECTORY})
	   message("                 " SPECTRA_INCLUDE_DIRS =  ${SPECTRA_INCLUDE_DIRS})
	   message("                 " SPECTRA_SOURCE_DIRS =  ${SPECTRA_SOURCE_DIRS})
	else()
	   message("+--------------------------------------------+")
	   message("| SPECTRA not found, please download SPECTRA |")
	   message("+--------------------------------------------+")
	   message(FATAL_ERROR "")
	endif()
endif()
