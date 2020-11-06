# - Try to find the ENTT library
# Once done this will define
#
#  ENTT_FOUND - system has ENTT
#  ENTT_INCLUDE_DIR - **the** ENTT include directory
#  ENTT_INCLUDE_DIRS - ENTT include directories
#  ENTT_SOURCES - the ENTT source files

if(NOT ENTT_FOUND)
    find_path(ENTT_INCLUDE_DIR
            NAMES entt/entt.hpp
            PATHS ${CMAKE_SOURCE_DIR}/external/entt/single_include/
            DOC "The entt include directory"
            NO_DEFAULT_PATH)

    if(ENTT_INCLUDE_DIR)
        set(ENTT_FOUND TRUE)
        set(ENTT_DIRECTORY ${CMAKE_SOURCE_DIR}/external/entt)
        set(ENTT_INCLUDE_DIRS ${ENTT_DIRECTORY}/single_include)
        set(ENTT_SOURCES_DIRS ${ENTT_DIRECTORY}/src/entt)

        message("entt found in " ENTT_DIRECTORY = ${ENTT_DIRECTORY})
        message("              " ENTT_INCLUDE_DIRS =  ${ENTT_INCLUDE_DIRS})
        message("              " ENTT_SOURCES_DIRS =  ${ENTT_SOURCES_DIRS})
    else()
        message("+--------------------------------------+")
        message("| ENTT not found, please download ENTT |")
        message("+--------------------------------------+")
        message(FATAL_ERROR "")
    endif()
endif()
