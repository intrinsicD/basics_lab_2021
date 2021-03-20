add_library(bcg_graphics STATIC)

target_link_libraries(bcg_graphics PUBLIC tbb)

add_subdirectory(color)
add_subdirectory(geometry)
add_subdirectory(math)
add_subdirectory(registration)
add_subdirectory(utils)
add_subdirectory(torch)

#target_link_options(bcg_graphics PUBLIC -v)

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
    message("CMAKE_CXX_COMPILER_VERSION " ${CMAKE_CXX_COMPILER_VERSION})

    target_link_libraries(bcg_graphics PUBLIC stdc++fs)
endif()

target_compile_features(bcg_graphics PUBLIC cxx_std_17)

target_include_directories(bcg_graphics PUBLIC "." "./geometry")

#target_compile_options(bcg_graphics PUBLIC "-Wall" "-Wextra")

if (UNIX AND NOT APPLE)
    #target_compile_options(bcg_graphics PUBLIC -Wall -Wno-shorten-64-to-32 -Wno-c++20-extensions -Wno-deprecated-copy -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
    #target_compile_options(bcg_graphics PUBLIC -Wall -Wpedantic -Wextra -Wno-missing-field-initializers -g -ggdb -fprofile-instr-generate -fcoverage-mapping -fstandalone-debug -O0)

    target_compile_options(bcg_graphics PUBLIC "-Wno-#pragma-messages")
    target_compile_options(bcg_graphics PUBLIC "-Wno-shorten-64-to-32")

    find_package(Threads REQUIRED)
    target_link_libraries(bcg_graphics PUBLIC Threads::Threads)
    #target_include_directories(bcg_graphics PUBLIC /usr/local/cuda-10.1/targets/x86_64-linux/include) // cuda support
endif ()

# warning flags
if (APPLE)
    #target_compile_options(bcg_graphics PUBLIC -Wall -Wno-deprecated -Wpedantic -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion -Wno-missing-field-initializers)
    target_compile_options(bcg_graphics PUBLIC "-Wno-#pragma-messages")
    target_compile_options(bcg_graphics PUBLIC "-Wno-shorten-64-to-32")
endif ()

if (MSVC)
    target_compile_options(bcg_graphics PUBLIC "/D_CRT_SECURE_NO_WARNINGS" "/wd4018" "/wd4244" "/wd4305" "/wd4800" "/wd4267" "/wd4838")
    target_compile_options(bcg_graphics PUBLIC "/EHsc")
    target_compile_definitions(bcg_graphics PUBLIC "NOMINMAX")
endif ()
