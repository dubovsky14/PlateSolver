#pragma once

#include<tuple>
#include<vector>
#include<map>
#include<string>
#include<memory>
#include<fstream>
#include <stdlib.h>

namespace PlateSolver   {
    typedef float CoordinateDataType;
    typedef std::tuple<CoordinateDataType, CoordinateDataType, CoordinateDataType, CoordinateDataType> PointCoordinatesTuple;
    typedef CoordinateDataType PointCoordinatesArray[4];
    typedef std::tuple<unsigned int, unsigned int, unsigned int, unsigned int> StarIndices;
    typedef int PointIndexType;

    class PointInKDTree {
        public:
            PointInKDTree();
            PointInKDTree(const PointCoordinatesTuple &coordinates, StarIndices &star_indices);

            PointIndexType get_child_index(const CoordinateDataType *coordinates);

            PointCoordinatesArray   m_coordinates;
            StarIndices             m_star_indices;
            short                   m_index_for_splitting   = -1;
            PointIndexType          m_index_child_left      = -1;
            PointIndexType          m_index_child_right     = -1;
            PointIndexType          m_index_parent          = -1;

    };

    /**
     * @brief Class implementing k-dimensional binary tree, which can be used to search for the nearest neighbors of a point
     *
     */
    class KDTree    {
        public:
            /**
             * @brief Construct a new KDTree object
             *
             * @param n_points approximate number of points that will be added (cannot be exceeded by any amount, but the performance will be slightly suboptimal)
             */
            KDTree(unsigned int n_points);

            /**
             * @brief Construct a new KDTree object from a binary file previously created by "save_to_file" method
             *
             * @param kd_tree_binary_file - binary file where the kd-tree is stored
             * @param cache_size - how many nodes should be stored in memory cache (the rest will be read from disk when looking for neighbors)
             */
            KDTree(const std::string &kd_tree_binary_file, unsigned int cache_size = 100000);

            /**
             * @brief Add point to the tree
             *
             * @param coordinates coordinates of the point in 4-dimensional space
             * @param star_indices indices of the stars describing this point (additional data that have to be stored)
             */
            void add_point(const PointCoordinatesTuple &coordinates, StarIndices &star_indices);

            /**
             * @brief Find all node conditions and create a tree structure for the tree
             *
             */
            void create_tree_structure();

            /**
             * @brief Get the k nearest neighbors to the query point
             *
             * @param query_point coordinates of the query point
             * @param n_points how many closest neighbors do you want
             * @return std::vector<std::tuple<PointCoordinatesTuple, StarIndices> > coordinates (hashes) and indices of the stars A,B,C and D from this point
             */
            std::vector<std::tuple<PointCoordinatesTuple, StarIndices> > get_k_nearest_neighbors(const PointCoordinatesTuple &query_point, unsigned int n_points);

            /**
             * @brief Get indices of the k nearest neighbors
             *
             * @param query_point  coordinates of the query point
             * @param n_points how many closest neighbors do you want
             * @return std::vector<unsigned int> - indices of the nodes (leafs) of the nearest neighbors
             */
            std::vector<unsigned int> get_k_nearest_neighbors_indices(const PointCoordinatesTuple &query_point, unsigned int n_points);

            /**
             * @brief Save the kd-tree to a binary file
             *
             * @param output_file_address  address of the output binary file
             */
            void save_to_file(const std::string &output_file_address)   const;

            void get_point(unsigned int node_index, PointCoordinatesArray coordinates, StarIndices *star_indices) const;

            unsigned int get_number_of_points_in_tree() const   {return m_number_of_points_in_tree;}

        private:

            int m_root_node_index = -1;

            std::vector<PointInKDTree> m_points_in_tree;
            unsigned int m_number_of_points_in_tree = 0;

            // build node and return its index
            int build_node(std::vector<unsigned int> *sub_indices, int parent_index);

            void split_based_on_cut(    const std::vector<unsigned int> &sub_indices, short coordinate_index,
                                        CoordinateDataType cut_value, unsigned int index_to_omit,
                                        std::vector<unsigned int> *indices_lower_value, std::vector<unsigned int> *indices_higher_value);

            CoordinateDataType get_median_and_its_index_from_sample(const std::vector<unsigned int> &sub_indices, short coordinate, unsigned int *median_index = nullptr)   const;

            static float get_distance(const CoordinateDataType *x1, const CoordinateDataType *x2);

            void scan_children_nodes(   unsigned int node_index,
                                        std::map<unsigned int, char> *visited_nodes,
                                        std::vector<std::tuple <unsigned int, float> > *vector_index_distance,
                                        const CoordinateDataType *query_point_array
                                        );

            static void add_node_to_vector_index_distance(float distance, std::vector<std::tuple <unsigned int, float> > *vector_index_distance, unsigned int n_points, unsigned int node_index);

            // for reading from disk implementation
            std::shared_ptr<std::ifstream> m_input_file = nullptr;
            void clean_cache();

            mutable std::map<unsigned int, PointInKDTree>   m_index_to_node_map;    // cache
            unsigned int                            m_points_in_map = 0;
            unsigned int                            m_chache_size   = 0;

            PointInKDTree get_point_in_tree(unsigned int node_index) const;


    };


    float RandomUniform();
}