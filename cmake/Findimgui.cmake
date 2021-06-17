# - Try to find the IMGUI library
# Once done this will define
#
#  IMGUI_FOUND - system has IMGUI
#  IMGUI_INCLUDE_DIR - **the** IMGUI include directory
#  IMGUI_INCLUDE_DIRS - IMGUI include directories
#  IMGUI_SOURCES - the IMGUI source files

if(NOT IMGUI_FOUND)
	find_path(IMGUI_INCLUDE_DIR
		NAMES imgui.h
	   	PATHS ${CMAKE_SOURCE_DIR}/external/imgui/
		DOC "The imgui include directory"
		NO_DEFAULT_PATH)

	if(IMGUI_INCLUDE_DIR)
	   set(IMGUI_FOUND TRUE)
	   set(IMGUI_DIRECTORY ${CMAKE_SOURCE_DIR}/external/imgui)
	   set(IMGUI_INCLUDE_DIRS ${IMGUI_DIRECTORY} ${IMGUI_DIRECTORY}/examples)
	   set(IMGUI_SOURCE_DIRS ${IMGUI_INCLUDE_DIRS} ${IMGUI_SOURCES})
	   set(IMGUI_SOURCES
			   ${IMGUI_DIRECTORY}/imgui.cpp
			   ${IMGUI_DIRECTORY}/imgui_demo.cpp
			   ${IMGUI_DIRECTORY}/imgui_draw.cpp
			   ${IMGUI_DIRECTORY}/imgui_tables.cpp
			   ${IMGUI_DIRECTORY}/imgui_widgets.cpp
			   ${IMGUI_DIRECTORY}/backends/imgui_impl_glfw.cpp
			   ${IMGUI_DIRECTORY}/backends/imgui_impl_opengl3.cpp
			   )

	   message("imgui found in " IMGUI_DIRECTORY = ${IMGUI_DIRECTORY})
	   message("               " IMGUI_INCLUDE_DIRS =  ${IMGUI_INCLUDE_DIRS})
	   message("               " IMGUI_SOURCE_DIRS =  ${IMGUI_SOURCE_DIRS})
	else()
	   message("+--------------------------------------- +")
	   message("| IMGUI not found, please download IMGUI |")
	   message("+--------------------------------------- +")
	   message(FATAL_ERROR "")
	endif()
endif()
