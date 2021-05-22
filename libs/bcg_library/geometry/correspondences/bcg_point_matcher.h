//
// Created by alex on 20.05.21.
//

#ifndef BCG_GRAPHICS_BCG_POINT_MATCHER_H
#define BCG_GRAPHICS_BCG_POINT_MATCHER_H

#include "math/matrix/bcg_matrix.h"
#include "math/bcg_math_types.h"
#include "correspondences/bcg_correspondences.h"
#include "kdtree/bcg_kdtree.h"

namespace bcg {

struct point_matcher {
    /*
     * Make Sampler class which can be added to point matcher to sample point clouds
     * Make Correspondence filter class which contains the {{... and ... } or ... or{... and ...}} filter sets for each batch
     *  - so this class somehow must know about the batches
     * Make Correspondence weighting class
     * Filters and weights should be applied during estimation if possible or afterwards on all correspondences.
     * */
    struct FilterKernel {
        enum class FilterType{
            identity,
            less,
            less_equal,
            greater_equal,
            greater,
            mutual,
            inconsistent
        }filter_type;
        bcg_scalar_t threshold;
    };

    struct WeightingKernel{
        enum class KernelType{
            identity,
            constant,
            gaussian,
            inverse,
            truncated,
            welsch
            //other robust kernel functions
        };
    };

    struct Features {
        struct Batch {
            using container_t = std::vector<std::string>;
            using iterator_t = container_t::iterator;
            using const_iterator_t = container_t::const_iterator;

            enum class MatchingDistance{
                L2,
                L1,
                dot
            }matching_distance;

            FilterKernel filter_kernel;
            std::string name;

            Batch();

            Batch(std::string name);

            Batch(std::string name, container_t batch);

            void add_property(int i);

            void add_property(const std::string &property_name);

            void remove_property(const std::string &property_name);

            void remove_property(int i);

            void set_batch(container_t batch);

            void clear();

            iterator_t begin();

            iterator_t end();

            const_iterator_t begin() const;

            const_iterator_t end() const;

            void build(const vertex_container &source, const vertex_container &target);

            correspondences match() const;

            bool test_filter(bcg_index_t vertex_index) const;

            correspondences apply_filter() const;
        private:
            bool is_build;
            MatrixS<-1, -1> source_batch_embedding;
            MatrixS<-1, -1> target_batch_embedding;
            kdtree_property<bcg_scalar_t> source_batch_index;
            kdtree_property<bcg_scalar_t> target_batch_index;
            container_t property_names;
        };

        using container_t = std::vector<Batch>;
        using iterator_t = container_t::iterator;
        using const_iterator_t = container_t::const_iterator;

        void add_batch(const std::string &name);

        void add_batch(const Batch &batch);

        void remove_batch(int i);

        void remove_batch(const std::string &name);

        void clear();

        iterator_t begin();

        iterator_t end();

        const_iterator_t begin() const;

        const_iterator_t end() const;

        void build(const vertex_container &source, const vertex_container &target);

        correspondences match() const;

        std::vector<bool> test_filters(bcg_index_t vertex_index) const;

        correspondences apply_filters() const;

    private:
        std::vector<std::vector<FilterKernel>> filter_kernels;
        WeightingKernel weighting_kernel;
        container_t matching_batches;
    }features;
    //TODO figure out how to exactly do this:
    //First select reference vertices on source and target
    //  -all
    //  -subset(random, voxel_grid-strategy, poisson)
    //  +/- feature-aware
    //During matching, filter (prune) correspondence and weight if it passes filtering.
    //Then select error metric to minimize (also look at m-estimators again)
    //Then select minimizer: point2point, point2plane, cayley
    //Finally select accelerator: anderson acceleration or not
};

}

#endif //BCG_GRAPHICS_BCG_POINT_MATCHER_H
