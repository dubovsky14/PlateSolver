#include "../PlateSolver/KDTree.h"

#include <algorithm>
#include <iostream>

using namespace PlateSolver;
using namespace std;


PointInKDTree::PointInKDTree(const PointCoordinatesTuple &coordinates, StarIndices &star_indices)   {
    m_coordinates[0]    = get<0>(coordinates);
    m_coordinates[1]    = get<1>(coordinates);
    m_coordinates[2]    = get<2>(coordinates);
    m_coordinates[3]    = get<3>(coordinates);
    m_star_indices      = star_indices;
};

KDTree::KDTree(unsigned int n_points)   {
    m_points_in_tree.reserve(n_points);
};

void KDTree::add_point(const PointCoordinatesTuple &coordinates, StarIndices &star_indices) {
    m_points_in_tree.push_back(PointInKDTree(coordinates, star_indices));
};

void KDTree::create_tree_structure()    {
    vector<unsigned int> all_indices(m_points_in_tree.size());
    for (unsigned int i = 0; i< m_points_in_tree.size(); i++)   {
        all_indices[i] = i;
    }
    m_root_node_index = build_node(all_indices, -1);
};

vector<std::tuple<PointCoordinatesTuple, StarIndices> > KDTree::get_k_nearest_neighbors(const PointCoordinatesTuple &query_point, unsigned int n_points)   {

};

int KDTree::build_node(const std::vector<unsigned int> &sub_indices, int parent_index) {

    if (m_nodes_built % 1000) cout << "Built " << m_nodes_built << " / " << m_points_in_tree.size() << " nodes, sub_indices.size() = " << sub_indices.size() << endl;
    m_nodes_built++;

    const bool has_parent = parent_index >= 0;
    short index_for_splitting = has_parent ? (m_points_in_tree[parent_index].m_index_for_splitting + 1) % 4 : 0;

    unsigned int this_point_index;
    const CoordinateDataType median = get_median_and_its_index_from_sample(sub_indices, index_for_splitting, &this_point_index);

    PointInKDTree &this_point = m_points_in_tree[this_point_index];

    this_point.m_index_for_splitting    = index_for_splitting;
    this_point.m_index_parent           = parent_index;

    vector<unsigned int> indices_child_left;
    vector<unsigned int> indices_child_right;
    indices_child_left.reserve(sub_indices.size()/2);
    indices_child_right.reserve(sub_indices.size()/2);
    split_based_on_cut(sub_indices, index_for_splitting, median, this_point_index, &indices_child_left, &indices_child_right);

    cout << "Children right = " << indices_child_right.size() << endl;
    cout << "Children left = " << indices_child_left.size() << endl;

    if (indices_child_left.size())  {
        this_point.m_index_child_left = build_node(indices_child_left, this_point_index);
    }
    if (indices_child_right.size()) {
        this_point.m_index_child_right = build_node(indices_child_right, this_point_index);
    }


    return this_point_index;
};

void KDTree::split_based_on_cut(const vector<unsigned int> &sub_indices, short coordinate_index,
                                CoordinateDataType cut_value, unsigned int index_to_omit,
                                vector<unsigned int> *indices_lower_value, vector<unsigned int> *indices_higher_value)  {
    for (unsigned int sub_index : sub_indices)  {
        if (sub_index == index_to_omit) {
            continue;
        }
        const PointInKDTree &this_point = m_points_in_tree.at(sub_index);
        const CoordinateDataType value_this_point = this_point.m_coordinates[coordinate_index];
        if (value_this_point > cut_value)   indices_higher_value->push_back(sub_index);
        else                                indices_lower_value->push_back(sub_index);
    }
};

CoordinateDataType KDTree::get_median_and_its_index_from_sample(const std::vector<unsigned int> &sub_indices, short coordinate, unsigned int *median_index)   const   {
    vector<unsigned int> subsample;
    if (sub_indices.size() < 200)   {
        subsample = sub_indices;
    }
    else {
        for (unsigned int i = 0; i < 200; i++)  {
            const unsigned int random_index = RandomUniform()*sub_indices.size();
            subsample.push_back(sub_indices[random_index]);
        }
    }

    const vector<PointInKDTree> &points_in_tree = m_points_in_tree;
    sort(subsample.begin(), subsample.end(), [points_in_tree, coordinate](unsigned int a, unsigned int b){
        return points_in_tree[a].m_coordinates[coordinate] > points_in_tree[b].m_coordinates[coordinate];
    });

    const unsigned int middle_index = subsample[subsample.size()/2];
    if (median_index) *median_index = middle_index;

    return points_in_tree[middle_index].m_coordinates[coordinate];
};


float PlateSolver::RandomUniform()  {
    // Be carefull about the brackets, otherwise RAND_MAX will overflow
    return (float(rand())) / (float(RAND_MAX)+1);
}