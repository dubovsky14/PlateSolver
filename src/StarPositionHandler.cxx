#include "../PlateSolver/StarPositionHandler.h"

#include <cmath>
#include <algorithm>

using namespace std;
using namespace PlateSolver;

StarPositionHandler::StarPositionHandler(unsigned int approximate_number_of_stars)   {
    reserve(approximate_number_of_stars);
};

StarPositionHandler::StarPositionHandler(const StarDatabaseHandler &star_database_handler)   {
    const unsigned int number_of_stars = star_database_handler.get_number_of_stars();
    reserve(number_of_stars);
    float RA, dec, mag;
    for (unsigned int i = 0; i < number_of_stars; i++)  {
        star_database_handler.get_star_info(i, &RA, &dec, &mag);
        AddStar(RA, dec,mag,i);
    }
};

void StarPositionHandler::AddStar(float RA, float dec, float mag, unsigned int id)  {
    const Vector3D star_vector(1, RA*(M_PI/12), dec*(M_PI/180), CoordinateSystem::enum_spherical);
    m_star_positions.push_back(star_vector);
    m_star_IDs.push_back(id);
    m_star_magnitudes.push_back(mag);
};

std::vector<std::tuple<Vector3D, float, unsigned int> >  StarPositionHandler::get_stars_around_coordinates(float RA, float dec, float angle, bool sort_by_magnitude)   const {
    const float cos_angle = cos(angle);
    const Vector3D selected_point(1, RA*(M_PI/12), dec*(M_PI/180), CoordinateSystem::enum_spherical);

    std::vector<std::tuple<Vector3D, float, unsigned int> > result;
    for (unsigned int i_star = 0; i_star < m_star_positions.size(); i_star++)   {
        const float scalar_product = selected_point.scalar_product(m_star_positions[i_star]);
        if (scalar_product > cos_angle) {
            result.push_back(tuple<Vector3D, float, unsigned int>  ({
                m_star_positions[i_star],
                m_star_magnitudes[i_star],
                m_star_IDs[i_star],
            }));
        }
    }

    if (sort_by_magnitude)  {
        sort(result.begin(), result.end(), [](const tuple<Vector3D, float, unsigned int> &a, const tuple<Vector3D, float, unsigned int> &b) {
            return get<2>(a) > get<2>(b);
        });
    }
    return result;
};

void StarPositionHandler::reserve(unsigned int approximate_number_of_stars){
    m_star_positions.reserve(approximate_number_of_stars);
    m_star_IDs.reserve(approximate_number_of_stars);
    m_star_magnitudes.reserve(approximate_number_of_stars);
};