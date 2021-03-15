add_subdirectory(spectra)
add_subdirectory(entt)

if(BCG_TESTS)
    add_subdirectory(googletest)
    include_directories(googletest/googletest/include)
endif(BCG_TESTS)
