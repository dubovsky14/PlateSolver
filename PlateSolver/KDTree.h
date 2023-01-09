#pragma once

#include<tuple>
#include<vector>
#include <stdlib.h>

namespace PlateSolver   {
    typedef float CoordinateDataType;
    typedef std::tuple<CoordinateDataType, CoordinateDataType, CoordinateDataType, CoordinateDataType> PointCoordinatesTuple;
    typedef CoordinateDataType PointCoordinatesArray[4];
    typedef std::tuple<unsigned int, unsigned int> StarIndices;
    typedef int PointIndexType;

    class PointInKDTree {
        public:
            PointInKDTree(const PointCoordinatesTuple &coordinates, StarIndices &star_indices);

            PointIndexType get_child_index(const CoordinateDataType *coordinates);

            PointCoordinatesArray   m_coordinates;
            StarIndices             m_star_indices;
            short                   m_index_for_splitting   = -1;
            PointIndexType          m_index_child_left      = -1;
            PointIndexType          m_index_child_right     = -1;
            PointIndexType          m_index_parent          = -1;

    };

    class KDTree    {
        public:
            KDTree(unsigned int n_points);

            void add_point(const PointCoordinatesTuple &coordinates, StarIndices &star_indices);

            void create_tree_structure();

            std::vector<std::tuple<PointCoordinatesTuple, StarIndices> > get_k_nearest_neighbors(const PointCoordinatesTuple &query_point, unsigned int n_points);

            std::tuple<PointCoordinatesTuple, StarIndices> get_k_nearest_neighbor(const PointCoordinatesTuple &query_point);

        private:
            int m_root_node_index = -1;

            std::vector<PointInKDTree> m_points_in_tree;

            // build node and return its index
            int build_node(const std::vector<unsigned int> &sub_indices, int parent_index);

            void split_based_on_cut(    const std::vector<unsigned int> &sub_indices, short coordinate_index,
                                        CoordinateDataType cut_value, unsigned int index_to_omit,
                                        std::vector<unsigned int> *indices_lower_value, std::vector<unsigned int> *indices_higher_value);

            CoordinateDataType get_median_and_its_index_from_sample(const std::vector<unsigned int> &sub_indices, short coordinate, unsigned int *median_index = nullptr)   const;

            unsigned int m_nodes_built = 0;
    };


    float RandomUniform();
}