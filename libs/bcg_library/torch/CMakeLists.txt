if(BCG_TORCH)
    target_sources(bcg_graphics PRIVATE
            bcg_torch_test.h
            )

    find_package(Torch REQUIRED PATHS ${CMAKE_SOURCE_DIR}/external/libtorch_release)
    target_link_libraries(bcg_graphics PUBLIC torch)
endif()