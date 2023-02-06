#include "../PlateSolver/NightSkyIndexer.h"
#include "../PlateSolver/AsterismHasher.h"
#include "../PlateSolver/GeometricTransformations.h"
#include "../PlateSolver/Common.h"

#include "../PlateSolver/StringOperations.h"
#include "../PlateSolver/StarDatabaseHandler.h"


#include<cmath>
#include<map>
#include<chrono>
#include<iostream>

using namespace std;
using namespace PlateSolver;

NightSkyIndexer::NightSkyIndexer(const std::string &star_catalogue)  {
    StarDatabaseHandler star_database_handler(star_catalogue);
    m_star_position_handler = make_shared<const StarPositionHandler>(star_database_handler);
};

NightSkyIndexer::NightSkyIndexer(shared_ptr<const StarPositionHandler> star_position_handler)  {
    m_star_position_handler = star_position_handler;
};

void NightSkyIndexer::create_index_file(const string &index_file, float focal_length)  {
    if (!EndsWith(index_file, ".kdtree")) {
        throw std::string("Unknown file extension. See README for more information.");
    }

    m_output_kd_tree = make_shared<KDTree>(10000000);

    loop_over_night_sky(focal_length);

    m_output_kd_tree->create_tree_structure();
    m_output_kd_tree->save_to_file(index_file);

};

void NightSkyIndexer::focal_length_to_field_of_view(float focal_length_mm, float *width_radians, float *height_radians)    {
    *width_radians = 2*atan(36/(2*focal_length_mm));
    *height_radians = 2*atan(24/(2*focal_length_mm));
};

// result is vector of tuples, tuple consists of "asterism hash tuple" (tuple of 4 floats), followed by 4 unsigned ints, corresponding to indices of the 4 hashed stars
void NightSkyIndexer::index_sky_region(  float RA, float dec, float angle,
                        std::map<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>, std::tuple<float,float,float,float> > *result)   {

    vector<tuple<Vector3D, float, unsigned int> >  stars_around = m_star_position_handler->get_stars_around_coordinates(RA, dec, angle, true);

    vector<Vector3D> star_positions;
    star_positions.reserve(stars_around.size());
    for (const tuple<Vector3D, float, unsigned int> &star : stars_around)   {
        star_positions.push_back(get<0>(star));
    }

    // positions of the stars how camera sensor will see them (projection to a plane)
    vector<tuple<float, float> > stars_sensor_based_coordinates = convert_star_coordinates_to_pixels_positions(star_positions, RA, dec);

    const unsigned int NSTARS = 6;
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

                    tuple<unsigned int, unsigned int, unsigned int, unsigned int> indices_tuple(get<2>(stars_around[ combination[starA] ]),
                                                                                                get<2>(stars_around[ combination[starB] ]),
                                                                                                get<2>(stars_around[ combination[starC] ]),
                                                                                                get<2>(stars_around[ combination[starD] ]));

                    (*result)[indices_tuple] = asterism_hash;
                }
            }
        }
    }
};

vector<tuple<float, float> > NightSkyIndexer::convert_star_coordinates_to_pixels_positions(const vector<Vector3D> &stars, float RA, float dec)  {
    vector<tuple<float, float> > result;

    RaDecToPixelCoordinatesConvertor ra_dec_to_pixel( RA, dec, 0, 1e-6, 1200, 800); // the last three numbers are arbitrary here

    for (const Vector3D &star_3d_vector : stars)    {
        result.push_back(ra_dec_to_pixel.convert_to_pixel_coordinates(star_3d_vector, ZeroZeroPoint::center));
    }

    return result;
};

void NightSkyIndexer::loop_over_night_sky(float focal_length) {
    std::map<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>, std::tuple<float,float,float,float> > result;
    float angle_width, angle_height;
    focal_length_to_field_of_view(focal_length, &angle_width, &angle_height);
    const float FOV_angle = (min(angle_width,angle_height)*0.5);
    cout << "Creating index file, FOV = " << convert_to_deg_min_sec(FOV_angle*180/M_PI) << endl;
    const float step_size_in_FOVs = 0.3;


    const auto time_start = chrono::high_resolution_clock::now();
    index_sky_region(0,90,FOV_angle, &result);
    const float dec_step = (180/M_PI)*FOV_angle*step_size_in_FOVs;
    for (float declination = 90; declination > -90; declination -= dec_step)   {
        const float circumference = 2*M_PI*cos(declination*(M_PI/180));
        const int n_steps = circumference/(FOV_angle*step_size_in_FOVs);
        if (n_steps == 0)   continue;
        cout << "declination : " << declination;
        const float RA_step_size = 24./n_steps;
        for (float right_ascension = 0; right_ascension < 24; right_ascension += RA_step_size)   {
            index_sky_region(right_ascension, declination,FOV_angle, &result);
        }
        const auto duration_from_start = chrono::duration_cast<chrono::seconds>(chrono::high_resolution_clock::now()-time_start);
        const float fraction_remaining = (sin(declination*M_PI/180)+1)/2;
        if (fraction_remaining != 1)    {
            cout << "\t\tcurrent duration = " << duration_from_start.count() << "\t\t";
            const float time_to_end = duration_from_start.count()*fraction_remaining/(1-fraction_remaining);
            const unsigned int hours   = time_to_end/3600;
            const unsigned int minutes = (time_to_end - hours*3600)/60;
            const unsigned int seconds = time_to_end - hours*3600 - minutes*60;
            cout << "\t\testimated time to end: " << hours << ":" << minutes << ":" << seconds;
        }
        cout << endl;
    }
    dump_hashes_to_outfile(result);
    result.clear();
};

void NightSkyIndexer::dump_hashes_to_outfile(const std::map<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>, std::tuple<float,float,float,float> > &result) {
    for (const auto &x : result)   {
        const tuple<unsigned int, unsigned int, unsigned int, unsigned int> &indices_tuple = x.first;
        const tuple<float,float,float,float> &hash = x.second;

        m_output_kd_tree->add_point(hash, indices_tuple);
    }
};
