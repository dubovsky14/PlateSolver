#include "../PlateSolver/NightSkyIndexer.h"
#include "../PlateSolver/AsterismHasher.h"


#include<cmath>

using namespace std;
using namespace PlateSolver;

NightSkyIndexer::NightSkyIndexer(shared_ptr<const StarPositionHandler> star_position_handler)  {
    m_star_position_handler = star_position_handler;
};

void NightSkyIndexer::create_index_file(const string &index_file, float focal_length)  {
    // TODO
};

void NightSkyIndexer::focal_length_to_field_of_view(float focal_length_mm, float *width_radians, float *height_radians)    {
    *width_radians = 2*atan(36/2*focal_length_mm);
    *height_radians = 2*atan(24/2*focal_length_mm);
};

// result is vector of tuples, tuple consists of "asterism hash tuple" (tuple of 4 floats), followed by 4 unsigned ints, corresponding to indices of the 4 hashed stars
void NightSkyIndexer::index_sky_region(  float RA, float dec, float angle,
                        vector<tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > *result)   {

    vector<tuple<Vector3D, float, unsigned int> >  stars_around = m_star_position_handler->get_stars_around_coordinates(RA, dec, angle, true);

    // these 5 lines would be one line in python :-(
    vector<const Vector3D *> star_positions;
    star_positions.reserve(stars_around.size());
    for (const tuple<Vector3D, float, unsigned int> &star : stars_around)   {
        star_positions.push_back(&get<0>(star));
    }

    // positions of the stars how camera sensor will see them (projection to a plane)
    Vector3D reference_axis(1,dec, -RA*(M_PI/12), CoordinateSystem::enum_spherical);
    vector<tuple<float, float> > stars_sensor_based_coordinates = convert_star_coordinates_to_pixels_positions(star_positions, reference_axis);

    vector<vector<unsigned int> > combinations({
        {0,1,2,3},
        {0,1,2,4},
        {0,1,3,4},
        {0,2,3,4},
        {1,2,3,4},
        {1,2,3,4},
        {2,3,4,5},
        {0,1,4,5},
        {0,2,4,5},
    });

    for (const vector<unsigned int> &combination : combinations)    {
        vector<tuple<float, float> > stars_to_hash;
        for (unsigned int i_star : combination) {
            if (i_star < stars_sensor_based_coordinates.size()) {
                stars_to_hash.push_back(stars_sensor_based_coordinates[i_star]);
            }
            if (stars_to_hash.size() == 4)  {
                std::tuple<float,float,float,float> asterism_hash;
                calculate_asterism_hash(stars_to_hash, &asterism_hash);
                result->push_back   (tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int>
                                        (asterism_hash,
                                        get<2>(stars_around[ combination[0] ]),
                                        get<2>(stars_around[ combination[1] ]),
                                        get<2>(stars_around[ combination[2] ]),
                                        get<2>(stars_around[ combination[3] ]))
                                    );
            }
        }
    }

};


vector<tuple<float, float> > NightSkyIndexer::convert_star_coordinates_to_pixels_positions(const vector<const Vector3D*> &stars, const Vector3D &reference_axis)  {
    vector<tuple<float, float> > result;

    const Vector3D rotated_x_axis = reference_axis;
    const Vector3D rotated_y_axis = (reference_axis.x() == 0 && reference_axis.y() == 0) ?
                                    Vector3D(1,0,0) :   // if reference axis is z-axis (it's arbitrary choice, so I took x-axis)
                                    (Vector3D(0,0,1)*reference_axis);
    const Vector3D rotated_z_axis = rotated_x_axis*rotated_y_axis;

    for (const Vector3D *star_3d_vector : stars)    {
        const float x_pos = -star_3d_vector->scalar_product(rotated_y_axis);
        const float y_pos = star_3d_vector->scalar_product(rotated_z_axis);
        result.push_back(tuple<float,float>(-x_pos, y_pos));
    }

    return result;
};

void NightSkyIndexer::loop_over_night_sky() {
    // TODO

};
