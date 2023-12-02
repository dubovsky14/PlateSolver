#include "../PlateSolver/StarPositionHandler.h"

#include <cmath>
#include <algorithm>
#include <tuple>

using namespace std;
using namespace PlateSolver;

StarPositionHandler::StarPositionHandler(unsigned int approximate_number_of_stars)   {
    m_kd_tree = make_unique<KDTree_t<float, std::tuple<Vector3D, float, unsigned int>>>(3);
};

StarPositionHandler::StarPositionHandler(const StarDatabaseHandler &star_database_handler)   {
    const unsigned int number_of_stars = star_database_handler.get_number_of_stars();
    m_kd_tree = make_unique<KDTree_t<float, std::tuple<Vector3D, float, unsigned int>>>(3);
    float RA, dec, mag;
    for (unsigned int i = 0; i < number_of_stars; i++)  {
        star_database_handler.get_star_info(i, &RA, &dec, &mag);
        AddStar(RA, dec,mag,i);
    }
    m_kd_tree->build_tree_structure();
};

void StarPositionHandler::AddStar(float RA, float dec, float mag, unsigned int id)  {
    const Vector3D star_vector(1, dec*(M_PI/180), RA*(-M_PI/12), CoordinateSystem::enum_spherical);
    const float coordinates[3] = {star_vector.x(), star_vector.y(), star_vector.z()};
    const tuple<Vector3D, float, unsigned int> star_info(
        star_vector,
        mag,
        id
    );
    m_kd_tree->add_value(coordinates, star_info);
};

void StarPositionHandler::BuildTree()   {
    m_kd_tree->build_tree_structure();
};

std::vector<std::tuple<Vector3D, float, unsigned int> >  StarPositionHandler::get_stars_around_coordinates(float RA, float dec, float angle, bool sort_by_magnitude)   const {
    const double sin_angle_half = sin(angle/2);
    const double distance = 2.*sin_angle_half;

    const Vector3D selected_point(1, dec*(M_PI/180), RA*(-M_PI/12), CoordinateSystem::enum_spherical);
    const float coordinates[3] = {selected_point.x(), selected_point.y(), selected_point.z()};
    vector<long long int> indices_kd_tree = m_kd_tree->get_nodes_closer_than_x(coordinates, distance);


    std::vector<std::tuple<Vector3D, float, unsigned int> > result;
    for (long long int index : indices_kd_tree)   {
        const KDTreeNode<float, std::tuple<Vector3D, float, unsigned int>> &node = m_kd_tree->get_node(index);
        const tuple<Vector3D, float, unsigned int> &node_info = node.m_value;
        result.push_back(node_info);
    }

    if (sort_by_magnitude)  {
        sort(result.begin(), result.end(), [](const tuple<Vector3D, float, unsigned int> &a, const tuple<Vector3D, float, unsigned int> &b) {
            return get<1>(b) > get<1>(a);
        });
    }
    return result;
};
