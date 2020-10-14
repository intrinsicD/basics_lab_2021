add_library(bcg_graphics
        bcg_commonio.h
        utils/bcg_file.h utils/bcg_file.cpp
        utils/bcg_dynamic_bitset.h
        utils/bcg_file_watcher.h
        utils/bcg_logger.h
        utils/bcg_path.h utils/bcg_path.cpp
        utils/bcg_stl_utils.h
        math/bcg_linalg.h
        geometry/bcg_aligned_box.h
        geometry/bcg_property.h
        geometry/bcg_property_map_eigen.h
        geometry/bcg_transforms.h
        geometry/bcg_point_cloud.h geometry/bcg_point_cloud.cpp
        geometry/bcg_graph.h geometry/bcg_graph.cpp
        geometry/bcg_mesh.h geometry/bcg_mesh.cpp geometry/bcg_meshio.h geometry/bcg_meshio.cpp
        exts/cgltf/cgltf.h exts/cgltf/cgltf_write.h
        exts/entt/single_include/entt/entt.hpp
        exts/stb/stb_image.h exts/stb/stb_image_resize.h exts/stb/stb_image_write.h
        exts/json/include/nlohmann/json.hpp
        exts/tinyexr/tinyexr.h
        )

set_target_properties(bcg_graphics PROPERTIES LINKER_LANGUAGE CXX CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
target_include_directories(bcg_graphics PRIVATE ./exts/entt/single_include)
target_include_directories(bcg_graphics PUBLIC ./exts/glm)

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