#include "../PlateSolver/PlateSolverTool.h"

#include "../PlateSolver/ImageLoader.h"
#include "../PlateSolver/AsterismHasher.h"
#include "../PlateSolver/StarFinder.h"
#include "../PlateSolver/Common.h"
#include "../PlateSolver/Vector3D.h"
#include "../PlateSolver/NightSkyIndexer.h"
#include "../PlateSolver/GeometricTransformations.h"
#include "../PlateSolver/StarPlotter.h"


#include<vector>
#include<string>
#include<tuple>
#include<cmath>
#include<iostream>
#include<algorithm>

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

    const vector<AsterismHashWithIndices> hashes_with_indices_from_photo = get_hashes_with_indices(stars, 8);
    const vector<AsterismHash>  hashes_from_photo = extract_hashes(hashes_with_indices_from_photo);

    for (const AsterismHashWithIndices &hash_with_indices : hashes_with_indices_from_photo) {
        const auto hash = get<0>(hash_with_indices);
        cout << "[ " << get<0>(hash) << ", " << get<1>(hash) << ", " << get<2>(hash) << ", " << get<3>(hash) << "] ";
        cout << ", " << get<1>(hash_with_indices) << ", " << get<2>(hash_with_indices) << ", " << get<3>(hash_with_indices) << ", " << get<4>(hash_with_indices) << endl;
    }

    const vector<vector<AsterismHashWithIndices> > similar_hashes = m_hash_finder->get_similar_hashes(hashes_from_photo,50);

    const vector<tuple<float,float,float> > stars_around_center = select_stars_around_point(stars,
                                                                                            m_image_width_pixels/2,
                                                                                            -m_image_height_pixels/2,
                                                                                            m_image_height_pixels/2);

    vector<tuple<float,float,float,float,float> > valid_hypotheses;
    for (unsigned int i_hash_photo = 0; i_hash_photo < hashes_with_indices_from_photo.size(); i_hash_photo++)    {
        for (unsigned int i_hash_similar = 0; i_hash_similar < similar_hashes[i_hash_photo].size(); i_hash_similar++)    {
            const unsigned int starA_index_photo = get<1>(hashes_with_indices_from_photo[i_hash_photo]);
            const unsigned int starB_index_photo = get<2>(hashes_with_indices_from_photo[i_hash_photo]);
            const float xpos_starA = get<0>(stars[starA_index_photo] );
            const float ypos_starA = get<1>(stars[starA_index_photo] );
            const float xpos_starB = get<0>(stars[starB_index_photo] );
            const float ypos_starB = get<1>(stars[starB_index_photo] );

            const unsigned int starA_database_index = get<1>(similar_hashes[i_hash_photo][i_hash_similar]);
            const unsigned int starB_database_index = get<2>(similar_hashes[i_hash_photo][i_hash_similar]);

            if (starA_database_index == 3991 || starB_database_index == 3991)  {
                cout << "Verifying correct coordinates!\n";
            }

            const auto hypothesis_coordinates = get_hypothesis_coordinates( xpos_starA, ypos_starA, starA_database_index,
                                                                            xpos_starB, ypos_starB, starB_database_index,
                                                                            m_image_width_pixels, m_image_height_pixels);

            const bool valid_hypotesis = validate_hypothesis(stars_around_center, hypothesis_coordinates, m_image_width_pixels, m_image_height_pixels);
            if (valid_hypotesis)    {
                valid_hypotheses.push_back(hypothesis_coordinates);
            }
        }
    }
    if (valid_hypotheses.size() == 0)   {
        return tuple<float,float,float,float,float>(0,0,0,0,0);
    }
    else {
        return valid_hypotheses[0];
    }
};


bool PlateSolverTool::validate_hypothesis(  const std::vector<std::tuple<float,float,float> > &stars_from_photo,
                                            const std::tuple<float,float,float,float,float> &hypothesis_coordinates,
                                            float image_width_pixels, float image_height_pixels)    {

    const float hypothesis_RA       = get<0>(hypothesis_coordinates);
    const float hypothesis_dec      = get<1>(hypothesis_coordinates);
    const float hypothesis_rot      = get<2>(hypothesis_coordinates);
    const float hypothesis_im_width = get<3>(hypothesis_coordinates);
    const float hypothesis_im_height= get<4>(hypothesis_coordinates);
    const float radians_per_pixel   = hypothesis_im_height/image_height_pixels;

    // maximal allowed distance in pixels between the rea position of the star (from database) and the position from photo to be considered as "matched"
    const float maximal_allowed_deviation2 = pow2(0.02*image_width_pixels);

    RaDecToPixelCoordinatesConvertor ra_dec_to_pixel( hypothesis_RA, hypothesis_dec, -hypothesis_rot,
                                                                radians_per_pixel, image_width_pixels, image_height_pixels);

    cout << "radians per pixel: " << radians_per_pixel << endl;

    // get the stars that we should see in circle around the center of the image, with the radius hypothesis_im_height/2
    const auto stars_from_database = m_star_position_handler->get_stars_around_coordinates( hypothesis_RA,
                                                                                            hypothesis_dec,
                                                                                            0.5*vec_size(hypothesis_im_width,hypothesis_im_height),
                                                                                            true);

    // firstly convert RA,dec to pixel coordinates and keep only those with coordinates inside the sensor
    vector<tuple<float,float,float> > brightest_stars_from_database_pixel_coordinates;
    brightest_stars_from_database_pixel_coordinates.reserve(stars_from_database.size());
    for (const tuple<Vector3D, float, unsigned int> &star : stars_from_database) {
        //cout << "DEBUG 1: " << get<0>(star).to_string(CoordinateSystem::enum_spherical) << "\n";
        const tuple<float,float> pixel_coor = ra_dec_to_pixel.convert_to_pixel_coordinates(get<0>(star), ZeroZeroPoint::upper_left);
        //cout << "DEBUG 2: " << get<0>(pixel_coor) << ", " <<  get<1>(pixel_coor) << "]\n";
        const float pos_x = get<0>(pixel_coor);
        const float pos_y = get<1>(pixel_coor);

        // skip stars outside of the sensor
        if (pos_x < 0 || pos_x > image_width_pixels || pos_y > 0 || -pos_y > image_height_pixels) continue;

        const float magnitude = get<1>(star);
        brightest_stars_from_database_pixel_coordinates.push_back(tuple<float,float,float>(pos_x, pos_y, magnitude));
    }

    // keep only 12 brightest stars from the photo (if available)
    std::vector<std::tuple<float,float,float> > brightest_stars_from_photo = stars_from_photo;
    if (brightest_stars_from_photo.size() > 12)   brightest_stars_from_photo.resize(12);
    const unsigned int n_stars_photo = brightest_stars_from_photo.size();

    // select 2 times more stars from database as we have from the photo (if available)
    if (brightest_stars_from_database_pixel_coordinates.size() > 2*n_stars_photo)   {
        brightest_stars_from_database_pixel_coordinates.resize(2*n_stars_photo);
    }

    unsigned int n_stars_truth_paired = 0;
    for (const auto &star_truth : brightest_stars_from_database_pixel_coordinates)  {
        bool paired_to_stars_from_photo = false;
        for (const auto &star_photo : brightest_stars_from_photo)  {
            if (calculate_dist2(star_truth, star_photo) < maximal_allowed_deviation2)   {
                paired_to_stars_from_photo = true;
            }
        }
        if (paired_to_stars_from_photo) n_stars_truth_paired++;
    }

    return n_stars_truth_paired > 0.5*n_stars_photo;
};

vector<AsterismHashWithIndices> PlateSolverTool::get_hashes_with_indices(const vector<tuple<float,float,float> > &stars, unsigned nstars)   {
    vector<AsterismHashWithIndices> result;
    unsigned int star_indices[4];
    for (star_indices[0] = 0; star_indices[0] < nstars; star_indices[0]++) {
        for (star_indices[1] = (star_indices[0])+1; star_indices[1] < nstars; star_indices[1]++) {
            for (star_indices[2] = star_indices[1]+1; star_indices[2] < nstars; star_indices[2]++) {
                for (star_indices[3] = star_indices[2]+1; star_indices[3] < nstars; star_indices[3]++) {
                    if (star_indices[3] >= stars.size())    {
                        break;
                    }

                    tuple<float,float,float,float> asterism_hash;
                    vector<tuple<float, float> > stars_to_hash = {
                        tuple(get<0>(stars[star_indices[0]]), get<1>(stars[star_indices[0]])),
                        tuple(get<0>(stars[star_indices[1]]), get<1>(stars[star_indices[1]])),
                        tuple(get<0>(stars[star_indices[2]]), get<1>(stars[star_indices[2]])),
                        tuple(get<0>(stars[star_indices[3]]), get<1>(stars[star_indices[3]])),
                    };
                    unsigned int starA,starB,starC,starD;
                    const bool valid_hash = calculate_asterism_hash(stars_to_hash, &asterism_hash,&starA,&starB,&starC,&starD);

                    if (!valid_hash)    {
                        continue;
                    }

                    result.push_back(AsterismHashWithIndices{asterism_hash, star_indices[starA],
                                                                            star_indices[starB],
                                                                            star_indices[starC],
                                                                            star_indices[starD]});
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

    //const vector<tuple<float,float>> pixel_coordinates = NightSkyIndexer::convert_star_coordinates_to_pixels_positions(vector<Vector3D>({starA_vector, starB_vector}), starA_vector);


    RaDecToPixelCoordinatesConvertor ra_dec_to_pixel_convertor( RA_starA, dec_starA, 0,angle_per_pixel, m_image_width_pixels, m_image_height_pixels);
    vector<tuple<float,float> > pixel_coordinates(2);
    pixel_coordinates[0] = ra_dec_to_pixel_convertor.convert_to_pixel_coordinates(starA_vector, ZeroZeroPoint::center);
    pixel_coordinates[1] = ra_dec_to_pixel_convertor.convert_to_pixel_coordinates(starB_vector, ZeroZeroPoint::center);

    // result
    const float rotation = get_angle(   pos_x_starB - pos_x_starA, pos_y_starB - pos_y_starA,
                                        get<0>(pixel_coordinates[1]), get<1>(pixel_coordinates[1]));

    PixelCoordinatesToRaDecConvertor convertor_center_in_starA(RA_starA, dec_starA, rotation, angle_per_pixel, image_width, image_height);
    const tuple<float,float> RA_dec_center = convertor_center_in_starA.convert_to_ra_dec(
        image_width/2-pos_x_starA,      // coordinates of the center of the image with respect to starA
        -image_height/2-pos_y_starA,    // coordinates of the center of the image with respect to starA
        ZeroZeroPoint::center
    );

    const float RA_center  = get<0>(RA_dec_center);
    const float dec_center = get<1>(RA_dec_center);

    return tuple<float,float,float,float,float>(RA_center, dec_center, rotation, result_width_angle, result_height_angle);
};

vector<tuple<float,float,float> > PlateSolverTool::select_stars_around_point(const vector<tuple<float,float,float> > &stars_all,
                                                                            float point_x, float point_y, float radius) {

    vector<tuple<float,float,float> > result;
    const float radius2 = radius*radius;
    for (const tuple<float,float,float> &star : stars_all)   {
        const float distance2 = pow2(get<0>(star) - point_x) + pow2(get<1>(star) - point_y);
        if (distance2 <= radius2)    {
            result.push_back(star);
        }
    }
    return result;
};

float PlateSolverTool::calculate_dist2(const tuple<float,float,float> &star1, const tuple<float,float,float> &star2) {
    return pow2(get<0>(star1) - get<0>(star2)) + pow2(get<1>(star1) - get<1>(star2));
};