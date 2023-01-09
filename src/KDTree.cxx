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

    if (m_nodes_built % 1000 == 0) cout << "Built " << m_nodes_built << " / " << m_points_in_tree.size() << " nodes, sub_indices.size() = " << sub_indices.size() << endl;
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

    //cout << "Children right = " << indices_child_right.size() << endl;
    //cout << "Children left = " << indices_child_left.size() << endl;
    //cin.get();

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
    // yes, this function is a mess, I know. But if you want to use processor cache and not to jump in the memory by few hundred MBs in each sort comparison, you don't have too much of a choice ...

    vector<unsigned int> subsample_indices;
    vector<unsigned int> indices_in_subsample_vector;
    if (sub_indices.size() < 200)   {
        subsample_indices = sub_indices;
    }
    else {
        for (unsigned int i = 0; i < 200; i++)  {
            const unsigned int random_index = RandomUniform()*sub_indices.size();
            subsample_indices.push_back(sub_indices[random_index]);
        }
    }
    indices_in_subsample_vector.resize(subsample_indices.size());

    // you definitely do not want to grab value from points_in_tree during sorting and jump randomly over hundreds MBs of memory ...
    vector<CoordinateDataType> subsample_values(subsample_indices.size());
    for (unsigned int i = 0; i < subsample_indices.size(); i++)  {
        indices_in_subsample_vector[i] = i;
        subsample_values[i] = m_points_in_tree[ subsample_indices[i] ].m_coordinates[coordinate];
    }

    sort(indices_in_subsample_vector.begin(), indices_in_subsample_vector.end(), [subsample_values](unsigned int a, unsigned int b){
        return subsample_values[a] > subsample_values[b];
    });

    vector<unsigned int> subsample_indices_sorted(subsample_indices.size());
    for (unsigned int i = 0; i < indices_in_subsample_vector.size(); i++)  {
        subsample_indices_sorted[i] = sub_indices[indices_in_subsample_vector[i]];
    }

    const unsigned int middle_index = subsample_indices_sorted[subsample_indices_sorted.size()/2];
    if (median_index) *median_index = middle_index;

    return m_points_in_tree[middle_index].m_coordinates[coordinate];
};


float PlateSolver::RandomUniform()  {
    // Be carefull about the brackets, otherwise RAND_MAX will overflow
    return (float(rand())) / (float(RAND_MAX)+1);
}