add_library(bcg_graphics
        bcg_common.h
        bcg_math.h bcg_color.h bcg_geometry.h
        bcg_noise.h bcg_sampling.h bcg_shading.h
        #yocto_modelio.h yocto_modelio.cpp
        #yocto_bvh.h yocto_bvh.cpp
        #yocto_shape.h yocto_shape.cpp
        #yocto_mesh.h yocto_mesh.cpp
        bcg_image.h bcg_image.cpp
        #yocto_trace.h yocto_trace.cpp
        #yocto_sceneio.h yocto_sceneio.cpp
        bcg_commonio.h bcg_commonio.cpp
        exts/stb/stb_image.h exts/stb/stb_image_resize.h exts/stb/stb_image_write.h
        exts/cgltf/cgltf.h exts/cgltf/cgltf_write.h
        exts/json/include/nlohmann/json.hpp
        exts/tinyexr/tinyexr.h
        )

set_target_properties(bcg_graphics PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

if (UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(bcg_graphics Threads::Threads)
endif (UNIX AND NOT APPLE)

# warning flags
if (APPLE)
    target_compile_options(bcg_graphics PUBLIC -Wall -Wconversion -Wno-sign-conversion -Wno-implicit-float-conversion)
endif (APPLE)
if (MSVC)
    target_compile_options(bcg_graphics PUBLIC /D_CRT_SECURE_NO_WARNINGS /wd4018 /wd4244 /wd4305 /wd4800 /wd4267)
    target_compile_options(bcg_graphics PUBLIC /EHsc)
endif (MSVC)