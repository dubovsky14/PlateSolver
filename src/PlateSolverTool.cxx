#include "../PlateSolver/PlateSolverTool.h"

#include "../PlateSolver/ImageLoader.h"
#include "../PlateSolver/AsterismHasher.h"
#include "../PlateSolver/StarFinder.h"
#include "../PlateSolver/Common.h"
#include "../PlateSolver/Vector3D.h"
#include "../PlateSolver/NightSkyIndexer.h"
#include "../PlateSolver/GeometricTransformations.h"


#include<vector>
#include<string>
#include<tuple>
#include<cmath>

using namespace PlateSolver;
using namespace std;


PlateSolverTool::PlateSolverTool(const string &hash_file, const string &stars_catalogue)  {
    m_star_database_handler = make_shared<StarDatabaseHandler>(stars_catalogue);
    m_hash_finder           = make_shared<HashFinder>(hash_file);
    m_star_position_handler = make_shared<StarPositionHandler>(*m_star_database_handler);
    m_night_sky_indexer     = make_shared<NightSkyIndexer>(m_star_position_handler);
};

tuple<float,float,float,float,float> PlateSolverTool::plate_solve(const string &jpg_file) {
    m_pixels = load_bw_image_to_uchar(jpg_file, &m_image_width_pixels);

    StarFinder star_finder(m_pixels, m_image_width_pixels);
    m_image_height_pixels = m_pixels.size()/m_image_width_pixels;

    const float brightness_threshold = star_finder.get_threshold(0.002);
    vector<tuple<float,float,float> > stars = star_finder.get_stars(brightness_threshold);

    const vector<AsterismHashWithIndices> hashes_with_indices_from_photo = get_hashes_with_indices(stars, 7);
    const vector<AsterismHash>  hashes_from_photo = extract_hashes(hashes_with_indices_from_photo);

    const vector<vector<AsterismHashWithIndices> > similar_hashes = m_hash_finder->get_similar_hashes(hashes_from_photo, 50);

    const vector<tuple<float,float,float> > stars_around_center = select_stars_around_point(stars,
                                                                                            m_image_width_pixels/2,
                                                                                            -m_image_height_pixels/2,
                                                                                            m_image_height_pixels/2);



};


bool PlateSolverTool::validate_hypothesis(  const std::vector<std::tuple<float,float,float> > &stars_around_center,
                                            const std::tuple<float,float,float,float,float> &hypothesis_coordinates)    {

    const float hypothesis_RA       = get<0>(hypothesis_coordinates);
    const float hypothesis_dec      = get<1>(hypothesis_coordinates);
    const float hypothesis_rot      = get<2>(hypothesis_coordinates);
    const float hypothesis_im_width = get<3>(hypothesis_coordinates);
    const float hypothesis_im_height= get<4>(hypothesis_coordinates);

    // get the stars that we should see in circle around the center of the image, with the radius hypothesis_im_height/2
    const std::vector<std::tuple<Vector3D, float, unsigned int> > stars_from_database = m_star_position_handler->get_stars_around_coordinates(hypothesis_RA, hypothesis_dec, hypothesis_im_height/2);

};

vector<AsterismHashWithIndices> PlateSolverTool::get_hashes_with_indices(const vector<tuple<float,float,float> > &stars, unsigned nstars)   {
    vector<AsterismHashWithIndices> result;
    for (unsigned int i_star1 = 0; i_star1 < nstars; i_star1++) {
        for (unsigned int i_star2 = i_star1+1; i_star2 < nstars; i_star2++) {
            for (unsigned int i_star3 = i_star2+1; i_star3 < nstars; i_star3++) {
                for (unsigned int i_star4 = i_star3+1; i_star4 < nstars; i_star4++) {
                    if (i_star4 >= stars.size())    {
                        break;
                    }

                    tuple<float,float,float,float> asterism_hash;
                    vector<tuple<float, float> > stars_to_hash = {
                        tuple(get<0>(stars[i_star1]), get<1>(stars[i_star1])),
                        tuple(get<0>(stars[i_star2]), get<1>(stars[i_star2])),
                        tuple(get<0>(stars[i_star3]), get<1>(stars[i_star3])),
                        tuple(get<0>(stars[i_star4]), get<1>(stars[i_star4])),
                    };
                    unsigned int starA,starB,starC,starD;
                    const bool valid_hash = calculate_asterism_hash(stars_to_hash, &asterism_hash,&starA,&starB,&starC,&starD);

                    if (!valid_hash)    {
                        continue;
                    }

                    result.push_back(AsterismHashWithIndices{asterism_hash, starA, starB, starC, starD});
                }
            }
        }
    }
    return result;
};

tuple<float,float,float,float,float> PlateSolverTool::get_hypothesis_coordinates(
                                                                        float pos_x_starA, float pos_y_starA, unsigned int id_starA,
                                                                        float pos_x_starB, float pos_y_starB, unsigned int id_starB,
                                                                        float image_width, float image_height)    {

    const float RA_starA  = m_star_database_handler->get_star_ra(id_starA);
    const float dec_starA = m_star_database_handler->get_star_dec(id_starA);
    const float RA_starB  = m_star_database_handler->get_star_ra(id_starB);
    const float dec_starB = m_star_database_handler->get_star_dec(id_starB);

    const Vector3D starA_vector = Vector3D::get_vector_unity_from_ra_dec(RA_starA, dec_starA);
    const Vector3D starB_vector = Vector3D::get_vector_unity_from_ra_dec(RA_starB, dec_starB);

    const float angle_A_B = Vector3D::get_angle(starA_vector, starB_vector);
    const float pixel_distance_A_B = sqrt( pow2(pos_x_starA - pos_x_starB) + pow2(pos_y_starA - pos_y_starB) );

    const float angle_per_pixel = angle_A_B/pixel_distance_A_B;

    // result
    const float result_width_angle  = angle_per_pixel*image_width;
    const float result_height_angle = angle_per_pixel*image_height;

    const vector<tuple<float,float>> pixel_coordinates = NightSkyIndexer::convert_star_coordinates_to_pixels_positions(vector<Vector3D>({starA_vector, starB_vector}), starA_vector);

    // result
    const float rotation = get_angle(   pos_x_starB - pos_x_starA, pos_y_starB - pos_y_starA,
                                        get<0>(pixel_coordinates[1]), get<1>(pixel_coordinates[1]));

    PixelCoordinatesToRaDecConvertor convertor_center_in_starA(RA_starA, dec_starA, rotation, angle_per_pixel, image_width, image_height);
    const tuple<float,float> RA_dec_center = convertor_center_in_starA.convert_to_ra_dec(
        image_width/2-pos_x_starA,      // coordinates of the center of the image with respect to starA
        -image_height/2-pos_x_starA,    // coordinates of the center of the image with respect to starA
        ZeroZeroPoint::center
    );

    const float RA_center  = get<0>(RA_dec_center);
    const float dec_center = get<1>(RA_dec_center);

    return tuple<float,float,float,float,float>(RA_center, dec_center, rotation, result_width_angle, result_height_angle);
};

vector<tuple<float,float,float> > select_stars_around_point(const vector<tuple<float,float,float> > &stars_all,
                                                            float point_x, float point_y, float radius) {

    vector<tuple<float,float,float> > result;
    const float radius2 = radius*radius;
    for (const tuple<float,float,float> star : stars_all)   {
        const float distance2 = pow2(get<0>(star) - point_x) + pow2(get<1>(star) - point_y);
        if (distance2 <= radius2)    {
            result.push_back(star);
        }
    }
    return result;
};
