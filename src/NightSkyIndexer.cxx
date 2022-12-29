#include "../PlateSolver/NightSkyIndexer.h"
#include "../PlateSolver/AsterismHasher.h"


#include<cmath>
#include<iostream>

using namespace std;
using namespace PlateSolver;

NightSkyIndexer::NightSkyIndexer(shared_ptr<const StarPositionHandler> star_position_handler)  {
    m_star_position_handler = star_position_handler;
};

void NightSkyIndexer::create_index_file(const string &index_file, float focal_length)  {
    m_output_hash_file = make_shared<ofstream>();
    m_output_hash_file->open(index_file);

    loop_over_night_sky(focal_length);

    m_output_hash_file->close();
};

void NightSkyIndexer::focal_length_to_field_of_view(float focal_length_mm, float *width_radians, float *height_radians)    {
    *width_radians = 2*atan(36/(2*focal_length_mm));
    *height_radians = 2*atan(24/(2*focal_length_mm));
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


    const unsigned int NSTARS = 7;
    unsigned int combination[4];
    for (unsigned int i_star1 = 0; i_star1 < NSTARS; i_star1++) {
        combination[0] = i_star1;
        for (unsigned int i_star2 = i_star1+1; i_star2 < NSTARS; i_star2++) {
            combination[1] = i_star2;
            for (unsigned int i_star3 = i_star2+1; i_star3 < NSTARS; i_star3++) {
                combination[2] = i_star3;
                for (unsigned int i_star4 = i_star3+1; i_star4 < NSTARS; i_star4++) {
                    combination[3] = i_star4;
                    if (i_star4 >= stars_sensor_based_coordinates.size()) {
                        break;
                    }
                    tuple<float,float,float,float> asterism_hash;
                    vector<tuple<float, float> > stars_to_hash = {
                        stars_sensor_based_coordinates[i_star1],
                        stars_sensor_based_coordinates[i_star2],
                        stars_sensor_based_coordinates[i_star3],
                        stars_sensor_based_coordinates[i_star4],
                    };
                    unsigned int starA,starB,starC,starD;
                    const bool valid_hash = calculate_asterism_hash(stars_to_hash, &asterism_hash,&starA,&starB,&starC,&starD);

                    if (!valid_hash)    {
                        continue;
                    }
                    result->push_back   (tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int>
                                            (asterism_hash,
                                            get<2>(stars_around[ combination[starA] ]),
                                            get<2>(stars_around[ combination[starB] ]),
                                            get<2>(stars_around[ combination[starC] ]),
                                            get<2>(stars_around[ combination[starD] ]))
                                        );

                }
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
        const float x_pos = star_3d_vector->scalar_product(rotated_x_axis);
        const float y_pos = star_3d_vector->scalar_product(rotated_y_axis);
        const float z_pos = star_3d_vector->scalar_product(rotated_z_axis);
        Vector3D rotated_vector(y_pos,z_pos,x_pos);
        result.push_back(tuple<float,float>(-rotated_vector.theta(), rotated_vector.phi()));
    }

    return result;
};

void NightSkyIndexer::loop_over_night_sky(float focal_length) {
    vector<tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > result;
    float angle_width, angle_height;
    focal_length_to_field_of_view(focal_length, &angle_width, &angle_height);
    const float FOV_angle = (angle_width*0.5);
    const float step_size_in_FOVs = 0.5;


    index_sky_region(0,90,FOV_angle, &result);
    const float dec_step = (180/M_PI)*FOV_angle*step_size_in_FOVs;
    for (float declination = 90; declination > -90; declination -= dec_step)   {
        const float circumference = 2*M_PI*cos(declination*(M_PI/180));
        const int n_steps = circumference/(FOV_angle*step_size_in_FOVs);
        if (n_steps == 0)   continue;
        cout << "declination : " << declination << endl;
        const float RA_step_size = 24./n_steps;
        for (float right_ascension = 0; right_ascension < 24; right_ascension += RA_step_size)   {
            index_sky_region(right_ascension, declination,FOV_angle, &result);
            dump_hash_vector_to_outfile(result);
            result.clear();
        }
    }

};

void NightSkyIndexer::dump_hash_vector_to_outfile(const std::vector<std::tuple<std::tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > &hash_vector) {
    for (const auto &x : hash_vector)   {
        const std::tuple<float,float,float,float> hash = get<0>(x);
        const unsigned int id_starA = get<1>(x);
        const unsigned int id_starB = get<2>(x);
        const unsigned int id_starC = get<3>(x);
        const unsigned int id_starD = get<4>(x);

        *m_output_hash_file << get<0>(hash) << "," << get<1>(hash) << ","  << get<2>(hash) << ","  << get<3>(hash) << ",";
        *m_output_hash_file << id_starA << "," << id_starB << "," << id_starC << "," << id_starD << endl;
    }
};
