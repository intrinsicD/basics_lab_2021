target_sources(bcg_graphics PRIVATE
        bcg_point_cloud.h bcg_point_cloud.cpp
        bcg_point_cloud_bilateral_filter.h bcg_point_cloud_bilateral_filter.cpp
        bcg_point_cloud_curvature_taubin.h bcg_point_cloud_curvature_taubin.cpp
        bcg_point_cloud_graph_builder.h bcg_point_cloud_graph_builder.cpp
        bcg_point_cloud_kernel_density_estimation.h bcg_point_cloud_kernel_density_estimation.cpp
        bcg_point_cloud_normal_filtering_robust_statistics.h bcg_point_cloud_normal_filtering_robust_statistics.cpp
        bcg_point_cloud_vertex_noise.h bcg_point_cloud_vertex_noise.cpp
        bcg_point_cloud_vertex_outlier_probability.h bcg_point_cloud_vertex_outlier_probability.cpp
        bcg_point_cloud_vertex_pca.h bcg_point_cloud_vertex_pca.cpp
        bcg_point_cloud_vertex_quadric.h bcg_point_cloud_vertex_quadric.cpp
        bcg_point_cloud_height_field.h bcg_point_cloud_height_field.cpp
        bcg_point_cloud_extremal_vertex_values.h bcg_point_cloud_extremal_vertex_values.cpp
        bcg_point_cloud_vertex_saliency.h bcg_point_cloud_vertex_saliency.cpp
        bcg_point_cloudio.h bcg_point_cloudio.cpp
        bcg_point_cloud_gaussians.h bcg_point_cloud_gaussians.cpp)
