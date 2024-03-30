#pragma once

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <tuple>
#include <cmath>
#include <fstream>
#include <memory>

namespace PlateSolver   {

    template<typename CoordinateType, typename ValueType>
    class KDTreeNode;

    template<typename CoordinateType, typename ValueType>
    class KDTree_t;

    template<typename CoordinateType, typename ValueType>
    class KDTreeStorageHandler;

    inline double pow2(const double x) {
        return x*x;
    }


    template<typename CoordinateType, typename ValueType>
    class KDTreeNode {
        public:
            KDTreeNode() = delete;
            KDTreeNode(int n_dim) {
                m_coordinates.resize(n_dim);
            };

            std::vector<CoordinateType> m_coordinates;
            ValueType                   m_value;
            int                         m_split_axis = -1;
            unsigned long long          m_left  = -1;
            unsigned long long          m_right = -1;
    };

    template<typename CoordinateType, typename ValueType>
    class KDTree_t    {
        public:
            explicit KDTree_t(int n_dim)  {m_n_dim = n_dim;};

            explicit KDTree_t(const std::string &file_address)   {
                m_storage_handler = std::make_shared<KDTreeStorageHandler<CoordinateType, ValueType>>(file_address);
                m_n_dim = m_storage_handler->get_n_dim();
                m_root_node_index = m_storage_handler->get_root_node_index();
                m_tree_structure_built = true;

                m_nodes = m_storage_handler->get_nodes();
                m_tree_structure_built = true;

            };

            KDTree_t() = delete;
            KDTree_t(const KDTree_t<CoordinateType, ValueType> &kd_tree) = delete;

            /**
             * @brief Get const reference to the node object, given index node_index
             *
             * @param node_index
             * @return const KDTreeNode<CoordinateType, ValueType>&
             */
            const KDTreeNode<CoordinateType, ValueType> &get_node(const long long int node_index) const {
                return m_nodes[node_index];
            };


            /**
             * @brief Get the number of nodes in the tree
             *
             * @return long long int
             */
            long long int get_n_nodes() const {
                return m_nodes.size();
            };

            /**
             * @brief Get index of the root node
             *
             */
            long long int get_root_node_index() const {
                return m_root_node_index;
            }

            /**
             * @brief Get const reference to the vector of nodes
             *
             * @return const std::vector<KDTreeNode<CoordinateType, ValueType>> &
             */
            const std::vector<KDTreeNode<CoordinateType, ValueType>> &get_nodes() const {
                return m_nodes;
            };


            /**
             * @brief Add point (node) to the tree. The tree structure must not be built yet.
             *
             * @param coordinates
             * @param value - these are the additional data associated with the node
             */
            void add_value(const std::vector<CoordinateType> &coordinates, const ValueType &value)    {
                if (m_tree_structure_built) {
                    throw std::runtime_error("KDTree::add_values: tree structure already built.");
                }

                if (coordinates.size() != m_n_dim) {
                    throw std::runtime_error("KDTree::add_value: coordinates must have m_n_dim elements.");
                }
                add_value(coordinates.data(), value);
            };


            /**
             * @brief Add point (node) to the tree. The tree structure must not be built yet.
             *
             * @param coordinates
             * @param value - these are the additional data associated with the node
             */
            void add_value(const CoordinateType *coordinates, const ValueType &value)   {
                KDTreeNode<CoordinateType, ValueType> node(m_n_dim);
                for (int i = 0; i < m_n_dim; ++i)  {
                    node.m_coordinates[i] = coordinates[i];
                }
                node.m_value = value;

                m_nodes.push_back(node);
            }

            /**
             * @brief Add points (nodes) to the tree. The tree structure must not be built yet.
             *
             * @param coordinates - vector of coordinates
             * @param values - vector of the additional data associated with the nodes
             */
            void add_values(const std::vector<std::vector<CoordinateType>> &coordinates, const std::vector<ValueType> &values)    {
                if (coordinates.size() != values.size())    {
                    throw std::runtime_error("KDTree::add_values: coordinates and values must have the same size.");
                }
                for (const auto &coordinate : coordinates)  {
                    if (coordinate.size() != m_n_dim)  {
                        throw std::runtime_error("KDTree::add_values: coordinates must have m_n_dim elements.");
                    }
                }

                for (int i = 0; i < coordinates.size(); ++i)  {
                    add_value(coordinates[i], values[i]);
                }
            };

            /**
             * @brief Get vector of indices of n closest points to the query point.
             *
             * @param coordinates of the query point
             * @param n - number of closest points to return
             */
            std::vector<long long int> get_n_closest_nodes(const CoordinateType *coordinates, const long long int n) const {
                if (!m_tree_structure_built) {
                    throw std::runtime_error("KDTree::get_n_closest_nodes: tree structure not built.");
                }
                if (n > m_nodes.size()) {
                    throw std::runtime_error("KDTree::get_n_closest_nodes: n must be smaller than the number of nodes.");
                }

                std::vector<std::tuple<long long int, double>> indices_and_distances;
                get_n_closest_nodes_recursive(coordinates, &indices_and_distances, n, m_root_node_index);

                std::vector<long long int> result;
                for (const auto &index_and_distance : indices_and_distances)  {
                    result.push_back(std::get<0>(index_and_distance));
                }
                return result;
            };

            /**
             * @brief Get vector of indices of all points closer than "distance" to the query point.
             *
             */
            std::vector<long long int> get_nodes_closer_than_x(const CoordinateType *coordinates, const double distance) const {
                if (!m_tree_structure_built) {
                    throw std::runtime_error("KDTree::get_nodes_closer_than_x: tree structure not built.");
                }

                std::vector<std::tuple<long long int, double>> indices_and_distances;
                get_nodes_closer_than_x_recursive(coordinates, &indices_and_distances, distance*distance, m_root_node_index);

                std::vector<long long int> result;
                for (const auto &index_and_distance : indices_and_distances)  {
                    result.push_back(std::get<0>(index_and_distance));
                }
                return result;
            }


            /**
             * @brief Create the tree structure from the added nodes. After this, no more nodes can be added.
             *
             */
            void build_tree_structure()   {
                if (m_tree_structure_built) {
                    return;
                }
                std::vector<long long int> node_indices;
                for (unsigned long long int i = 0; i < m_nodes.size(); ++i)  {
                    node_indices.push_back(i);
                }

                m_root_node_index = build_node(node_indices, 0);
                m_tree_structure_built = true;
            };


        private:
            bool m_tree_structure_built = false;
            std::vector<KDTreeNode<CoordinateType, ValueType>> m_nodes;
            std::shared_ptr<KDTreeStorageHandler<CoordinateType, ValueType>> m_storage_handler = nullptr;
            long long int m_root_node_index = -1;
            int m_n_dim = 0;

            /**
             * @brief Takes a vector of node indices and builds a root node and its child nodes from them. Returns the index of the root node.
             *
             * @param node_indices - vector of node indices
             * @param split_axis - axis along which to split the nodes
             * @return long long int - index of the build node ("root" node)
             */
            long long int build_node(const std::vector<long long int> &node_indices, const int split_axis)    {
                if (node_indices.size() == 0)   {
                    return -1;
                }
                if (node_indices.size() == 1)   {
                    m_nodes[node_indices[0]].m_split_axis = split_axis;
                    return node_indices[0];
                }

                std::vector<std::tuple<long long int, CoordinateType>> node_indices_and_coordinates;
                for (long long int node_index : node_indices)  {
                    node_indices_and_coordinates.push_back(std::make_tuple(node_index, m_nodes[node_index].m_coordinates[split_axis]));
                }

                std::sort(node_indices_and_coordinates.begin(), node_indices_and_coordinates.end(), [](const std::tuple<long long int, CoordinateType> &a, const std::tuple<long long int, CoordinateType> &b) {
                    return std::get<1>(a) < std::get<1>(b);
                });

                const long long int median_node_index = std::get<0>(node_indices_and_coordinates[node_indices_and_coordinates.size() / 2]);
                const CoordinateType median_coordinate = std::get<1>(node_indices_and_coordinates[node_indices_and_coordinates.size() / 2]);

                node_indices_and_coordinates.clear();

                m_nodes[median_node_index].m_split_axis = split_axis;

                std::vector<long long int> left_node_indices;
                std::vector<long long int> right_node_indices;
                for (const long long int i_node : node_indices)  {
                    if (i_node == median_node_index)    {
                        continue;
                    }
                    if (m_nodes[i_node].m_coordinates[split_axis] < median_coordinate)    {
                        left_node_indices.push_back(i_node);
                    }
                    else    {
                        right_node_indices.push_back(i_node);
                    }
                }

                m_nodes[median_node_index].m_left  = build_node(left_node_indices,  (split_axis + 1) % m_n_dim);
                m_nodes[median_node_index].m_right = build_node(right_node_indices, (split_axis + 1) % m_n_dim);

                return median_node_index;
            };

            void get_n_closest_nodes_recursive( const CoordinateType *coordinates,
                                                std::vector<std::tuple<long long int, double>> *node_indices_and_distances,
                                                int n_neighbors,
                                                long long int node_index) const {

                if (node_index == -1)   {
                    return;
                }

                const KDTreeNode<CoordinateType, ValueType> &node = m_nodes[node_index];
                const double distance_to_node = get_distance_squared(coordinates, node.m_coordinates.data());

                if (node_indices_and_distances->size() < n_neighbors) {
                    node_indices_and_distances->push_back(std::tuple<long long int, double>(node_index, distance_to_node));
                    sort(node_indices_and_distances->begin(), node_indices_and_distances->end(), [](const auto &a, const auto &b) {
                        return std::get<1>(a) < std::get<1>(b);
                    });
                }
                else if (distance_to_node < std::get<1>(node_indices_and_distances->back())) {
                    (*node_indices_and_distances)[n_neighbors-1] = std::tuple<long long int, double>(node_index, distance_to_node);
                    sort(node_indices_and_distances->begin(), node_indices_and_distances->end(), [](const auto &a, const auto &b) {
                        return std::get<1>(a) < std::get<1>(b);
                    });
                }

                const bool go_to_left = coordinates[node.m_split_axis] < node.m_coordinates[node.m_split_axis];
                if (go_to_left) {
                    get_n_closest_nodes_recursive(coordinates, node_indices_and_distances, n_neighbors, node.m_left);
                }
                else    {
                    get_n_closest_nodes_recursive(coordinates, node_indices_and_distances, n_neighbors, node.m_right);
                }

                const double distance_to_split_axis = pow2(coordinates[node.m_split_axis] - node.m_coordinates[node.m_split_axis]);
                if (distance_to_split_axis < std::get<1>(node_indices_and_distances->back()) || node_indices_and_distances->size() < n_neighbors) {
                    if (go_to_left) {
                        get_n_closest_nodes_recursive(coordinates, node_indices_and_distances, n_neighbors, node.m_right);
                    }
                    else    {
                        get_n_closest_nodes_recursive(coordinates, node_indices_and_distances, n_neighbors, node.m_left);
                    }
                }
            };


            void get_nodes_closer_than_x_recursive( const CoordinateType *coordinates,
                                                std::vector<std::tuple<long long int, double>> *node_indices_and_distances,
                                                double distance_squared,
                                                long long int node_index) const {

                if (node_index == -1)   {
                    return;
                }

                const KDTreeNode<CoordinateType, ValueType> &node = m_nodes[node_index];
                const double distance_to_node = get_distance_squared(coordinates, node.m_coordinates.data());

                if (distance_to_node < distance_squared) {
                    node_indices_and_distances->push_back(std::tuple<long long int, double>(node_index, distance_to_node));
                }

                const bool go_to_left = coordinates[node.m_split_axis] < node.m_coordinates[node.m_split_axis];
                if (go_to_left) {
                    get_nodes_closer_than_x_recursive(coordinates, node_indices_and_distances, distance_squared, node.m_left);
                }
                else    {
                    get_nodes_closer_than_x_recursive(coordinates, node_indices_and_distances, distance_squared, node.m_right);
                }

                const double distance_to_split_axis = pow2(coordinates[node.m_split_axis] - node.m_coordinates[node.m_split_axis]);
                if (distance_to_split_axis < distance_squared) {
                    if (go_to_left) {
                        get_nodes_closer_than_x_recursive(coordinates, node_indices_and_distances, distance_squared, node.m_right);
                    }
                    else    {
                        get_nodes_closer_than_x_recursive(coordinates, node_indices_and_distances, distance_squared, node.m_left);
                    }
                }
            };

            double get_distance_squared(const CoordinateType *coordinates_1, const CoordinateType *coordinates_2)   const    {
                double distance = 0.0;
                for (int i = 0; i < m_n_dim; ++i)  {
                    distance += (coordinates_1[i] - coordinates_2[i]) * (coordinates_1[i] - coordinates_2[i]);
                }
                return distance;
            };
    };




    template<typename CoordinateType, typename ValueType>
    class KDTreeStorageHandler  {
        public:
            KDTreeStorageHandler(const std::string &file_address)   {
                m_file = std::make_unique<std::ifstream>(file_address, std::ios::binary | std::ios::out);
                read_header();
            };

            KDTreeNode<CoordinateType, ValueType> get_node(const long long int node_index) const    {
                if (node_index < 0 || node_index >= m_n_nodes) {
                    throw std::runtime_error("KDTreeStorageHandler::get_node: node_index out of range.");
                }

                m_file->seekg(m_header_size + node_index * m_node_size);
                KDTreeNode<CoordinateType, ValueType> node(m_n_dim);
                m_file->read(reinterpret_cast<char *>(node.m_coordinates.data()), m_n_dim * sizeof(CoordinateType));
                m_file->read(reinterpret_cast<char *>(&node.m_value), sizeof(ValueType));
                m_file->read(reinterpret_cast<char *>(&node.m_split_axis), sizeof(int));
                m_file->read(reinterpret_cast<char *>(&node.m_left), sizeof(long long int));
                m_file->read(reinterpret_cast<char *>(&node.m_right), sizeof(long long int));

                return node;
            };

            std::vector<KDTreeNode<CoordinateType, ValueType>> get_nodes() const    {
                std::vector<KDTreeNode<CoordinateType, ValueType>> nodes;
                nodes.reserve(m_n_nodes);
                for (long long int i_node = 0; i_node < m_n_nodes; ++i_node)  {
                    nodes.push_back(get_node(i_node));
                }
                return nodes;
            };

            int get_n_dim() const   {
                return m_n_dim;
            };

            long long int get_n_nodes() const   {
                return m_n_nodes;
            };

            long long int get_root_node_index() const   {
                return m_root_node_index;
            };

            static void save_tree(const std::string &file_address, const KDTree_t<CoordinateType, ValueType> &kd_tree)   {
                const auto &nodes = kd_tree.get_nodes();
                const long long int n_nodes = nodes.size();
                if (n_nodes == 0)   {
                    throw std::runtime_error("KDTreeStorageHandler::save_tree: no nodes in the tree.");
                }

                const int n_dim = nodes[0].m_coordinates.size();
                const size_t header_size = sizeof(m_header_size) + sizeof(m_n_dim) + sizeof(m_node_size) + sizeof(m_n_nodes) + sizeof(m_root_node_index);
                const size_t node_size = n_dim * sizeof(CoordinateType) + sizeof(ValueType) + sizeof(nodes[0].m_split_axis) + 2 * sizeof(nodes[0].m_left);
                const long long int root_node_index = kd_tree.get_root_node_index();

                std::ofstream file(file_address, std::ios::binary | std::ios::out);
                file.write(reinterpret_cast<const char *>(&header_size), sizeof(m_header_size));
                file.write(reinterpret_cast<const char *>(&n_dim), sizeof(m_n_dim));
                file.write(reinterpret_cast<const char *>(&node_size), sizeof(m_node_size));
                file.write(reinterpret_cast<const char *>(&n_nodes), sizeof(m_n_nodes));
                file.write(reinterpret_cast<const char *>(&root_node_index), sizeof(m_root_node_index));

                for (const auto &node : nodes)  {
                    file.write(reinterpret_cast<const char *>(node.m_coordinates.data()), n_dim * sizeof(CoordinateType));
                    file.write(reinterpret_cast<const char *>(&node.m_value),       sizeof(ValueType));
                    file.write(reinterpret_cast<const char *>(&node.m_split_axis),  sizeof(node.m_split_axis));
                    file.write(reinterpret_cast<const char *>(&node.m_left),        sizeof(node.m_left));
                    file.write(reinterpret_cast<const char *>(&node.m_right),       sizeof(node.m_right));
                }
            };

        private:
            void read_header()  {
                m_file->read(reinterpret_cast<char *>(&m_header_size), sizeof(m_header_size));
                m_file->read(reinterpret_cast<char *>(&m_n_dim), sizeof(m_n_dim));
                m_file->read(reinterpret_cast<char *>(&m_node_size), sizeof(m_node_size));
                m_file->read(reinterpret_cast<char *>(&m_n_nodes), sizeof(m_n_nodes));
                m_file->read(reinterpret_cast<char *>(&m_root_node_index), sizeof(m_root_node_index));
            };

            std::unique_ptr<std::ifstream> m_file = nullptr;

            size_t          m_header_size   = 0;
            int             m_n_dim         = 0;
            size_t          m_node_size     = 0;
            long long int   m_n_nodes       = 0;
            long long int   m_root_node_index = -1;

    };

}