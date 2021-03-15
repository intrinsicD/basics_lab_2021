add_library(bcg_graphics STATIC)

find_package(TBB REQUIRED)

target_include_directories(bcg_graphics PUBLIC "${TBB_INCLUDE_DIRS}")

target_link_libraries(bcg_graphics PUBLIC tbb stdc++fs)

add_subdirectory(color)
add_subdirectory(geometry)
add_subdirectory(math)
add_subdirectory(registration)
add_subdirectory(utils)

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
    message("CMAKE_CXX_COMPILER_VERSION " ${CMAKE_CXX_COMPILER_VERSION})
    set_target_properties(bcg_opengl PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
endif()

target_compile_features(bcg_graphics PRIVATE cxx_std_17)

target_include_directories(bcg_graphics PUBLIC . ./geometry)

if (UNIX AND NOT APPLE)
    #target_compile_options(bcg_graphics PUBLIC -Wall -Wno-shorten-64-to-32 -Wno-c++20-extensions -Wno-deprecated-copy -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
    #target_compile_options(bcg_graphics PUBLIC -Wall -Wpedantic -Wextra -Wno-missing-field-initializers -g -ggdb -fprofile-instr-generate -fcoverage-mapping -fstandalone-debug -O0)
    target_compile_options(bcg_graphics PUBLIC "-Wno-#pragma-messages")
    target_compile_options(bcg_graphics PUBLIC "-Wno-shorten-64-to-32")

    find_package(Threads REQUIRED)
    target_link_libraries(bcg_graphics PUBLIC Threads::Threads)
    #target_include_directories(bcg_graphics PUBLIC /usr/local/cuda-10.1/targets/x86_64-linux/include) // cuda support
endif (UNIX AND NOT APPLE)

# warning flags
if (APPLE)
    #target_compile_options(bcg_graphics PUBLIC -Wall -Wno-deprecated -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
    target_compile_options(bcg_graphics PUBLIC "-Wno-#pragma-messages")
    target_compile_options(bcg_graphics PUBLIC "-Wno-shorten-64-to-32")
endif (APPLE)
if (MSVC)
    target_compile_options(bcg_graphics PUBLIC /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4244 /wd4305 /wd4800 /wd4267 /wd4838)
    target_compile_options(bcg_graphics PUBLIC /EHsc)
    target_compile_definitions(bcg_graphics PUBLIC NOMINMAX)
endif (MSVC)
