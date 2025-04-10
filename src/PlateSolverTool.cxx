#include "../PlateSolver/PlateSolverTool.h"

#include "../PlateSolver/AsterismHasher.h"
#include "../PlateSolver/StarFinder.h"
#include "../PlateSolver/Common.h"
#include "../PlateSolver/Vector3D.h"
#include "../PlateSolver/NightSkyIndexer.h"
#include "../PlateSolver/GeometricTransformations.h"
#include "../PlateSolver/StarPlotter.h"
#include "../PlateSolver/Logger.h"


#include<vector>
#include<string>
#include<tuple>
#include<cmath>
#include<iostream>
#include<algorithm>

using namespace PlateSolver;
using namespace std;


PlateSolverTool::PlateSolverTool(const string &stars_catalogue)  {
    Logger::log_message(bench_mark("Going to read catalogue and declare all handlers"));
    m_star_database_handler = make_shared<StarDatabaseHandler>(stars_catalogue);
    m_star_position_handler = make_shared<StarPositionHandler>(*m_star_database_handler);
    m_night_sky_indexer     = make_unique<NightSkyIndexer>(m_star_position_handler);
};

PlateSolverTool::PlateSolverTool(   std::shared_ptr<StarDatabaseHandler> star_database_handler,
                                    std::shared_ptr<StarPositionHandler> star_position_handler) {

    m_star_database_handler = star_database_handler;
    m_star_position_handler = star_position_handler;
    m_night_sky_indexer     = make_unique<NightSkyIndexer>(m_star_position_handler);
};

void PlateSolverTool::set_hash_file(const std::string &hash_file)   {
    m_hash_finder = make_unique<HashFinder>(hash_file);
};

tuple<float,float,float,float,float> PlateSolverTool::plate_solve(const string &jpg_file, const std::string &hash_file) {
    set_hash_file(hash_file);
    Logger::log_message(bench_mark("Going to load the image " + jpg_file));
    StarFinder star_finder(jpg_file);
    Logger::log_message(bench_mark("Image loaded " + jpg_file));
    return plate_solve(star_finder);
};

tuple<float,float,float,float,float> PlateSolverTool::plate_solve(const cv::Mat &photo, const std::string &hash_file) {
    set_hash_file(hash_file);
    StarFinder star_finder(photo);
    return plate_solve(star_finder);
};

tuple<float,float,float,float,float> PlateSolverTool::plate_solve(const StarFinder &star_finder) {
    m_image_height_pixels   = star_finder.get_height();
    m_image_width_pixels    = star_finder.get_width();

    const float brightness_threshold = star_finder.get_threshold(0.0005);
    Logger::log_message(bench_mark("Threshold calculated: " + to_string(brightness_threshold)));
    vector<tuple<float,float,float> > stars = star_finder.get_stars(brightness_threshold);
    Logger::log_message(bench_mark("Stars positions from photo calculated"));

    const vector<unsigned int> stars_to_consider_vector{8,10};    // in most of the cases hashes built from 6 stars are enough to plate-solve
    unsigned int previous_n_stars = 0;
    for (const unsigned int stars_to_consider : stars_to_consider_vector)   {
        vector<AsterismHashWithIndices> hashes_with_indices_from_photo = get_hashes_with_indices(stars, stars_to_consider, previous_n_stars);
        previous_n_stars = stars_to_consider;
        // sort hashes by sum of indices, so that hashes with brightest stars are first
        sort(hashes_with_indices_from_photo.begin(), hashes_with_indices_from_photo.end(),
                [](const AsterismHashWithIndices &a, const AsterismHashWithIndices &b)  {return (
                    (get<1>(a) + get<2>(a) + get<3>(a) + get<4>(a)) < (get<1>(b) + get<2>(b) + get<3>(b) + get<4>(b))
                );} );

        Logger::log_message(bench_mark("Hashes extracted and sorted. Using " + to_string(stars_to_consider) + " stars"));
        const vector<AsterismHash>  hashes_from_photo = extract_hashes(hashes_with_indices_from_photo);
        std::vector<std::tuple<unsigned int, unsigned int,float> > ordering_by_distance;

        // for each hash from photo, find 5 most similar hashes from kd-tree file
        const vector<vector<AsterismHashWithIndices> > similar_hashes = m_hash_finder->get_similar_hashes(hashes_from_photo,5, &ordering_by_distance);
        Logger::log_message(bench_mark("Similar hashes extracted. "));


        unsigned int i_attempt = 0;
        for (const tuple<unsigned int, unsigned int,float> &indexes_and_distance : ordering_by_distance)    {
            i_attempt++;
            if (i_attempt > 20) {
                break;  // in most of the cases (more than 90%), the first hash is the correct one, if it's not among first 20, almost certainly it's not there at all
            }
            const unsigned int i_hash_photo     = get<0>(indexes_and_distance);
            const unsigned int i_hash_similar   = get<1>(indexes_and_distance);
            const unsigned int starA_index_photo = get<1>(hashes_with_indices_from_photo[i_hash_photo]);
            const unsigned int starB_index_photo = get<2>(hashes_with_indices_from_photo[i_hash_photo]);
            const float xpos_starA = get<0>(stars[starA_index_photo] );
            const float ypos_starA = get<1>(stars[starA_index_photo] );
            const float xpos_starB = get<0>(stars[starB_index_photo] );
            const float ypos_starB = get<1>(stars[starB_index_photo] );

            const unsigned int starA_database_index = get<1>(similar_hashes[i_hash_photo][i_hash_similar]);
            const unsigned int starB_database_index = get<2>(similar_hashes[i_hash_photo][i_hash_similar]);


            const auto hypothesis_coordinates = get_hypothesis_coordinates( xpos_starA, ypos_starA, starA_database_index,
                                                                            xpos_starB, ypos_starB, starB_database_index,
                                                                            m_image_width_pixels, m_image_height_pixels);


            const bool valid_hypotesis = validate_hypothesis(stars, hypothesis_coordinates, m_image_width_pixels, m_image_height_pixels);
            if (valid_hypotesis)    {
                Logger::log_message(bench_mark("Correct hypothesis found after " + to_string(i_attempt) + " attempts"));
                return hypothesis_coordinates;
            }
            if (i_attempt == 1) {
                string message =    "The first hypothesis: RA = " + convert_to_deg_min_sec(get<0>(hypothesis_coordinates),"h")
                                    + " dec = " + convert_to_deg_min_sec(get<1>(hypothesis_coordinates))
                                    + " rot = " + convert_to_deg_min_sec((180/M_PI)*get<2>(hypothesis_coordinates))
                                    + " width = " + convert_to_deg_min_sec((180/M_PI)*get<3>(hypothesis_coordinates));
                Logger::log_message(message);
            }
        }
        Logger::log_message(bench_mark("No correct hypothesis was found for " + to_string(stars_to_consider) + " stars, " + to_string(i_attempt) + " hashes have been tried."));
    }
    return tuple<float,float,float,float,float>(0,0,0,0,0);
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

    // maximal allowed distance in pixels between the position of the star (from database) and the position from photo to be considered as "matched"
    const float maximal_allowed_deviation2 = pow2(0.01*image_width_pixels);

    RaDecToPixelCoordinatesConvertor ra_dec_to_pixel( hypothesis_RA, hypothesis_dec, -hypothesis_rot,
                                                                radians_per_pixel, image_width_pixels, image_height_pixels);


    // get the stars that we should see in circle around the center of the image, with the radius hypothesis_im_height/2
    const auto stars_from_database = m_star_position_handler->get_stars_around_coordinates( hypothesis_RA,
                                                                                            hypothesis_dec,
                                                                                            0.5*vec_size(hypothesis_im_width,hypothesis_im_height),
                                                                                            true);

    // firstly convert RA,dec to pixel coordinates and keep only those with coordinates inside the sensor
    vector<tuple<float,float,float> > brightest_stars_from_database_pixel_coordinates;
    brightest_stars_from_database_pixel_coordinates.reserve(stars_from_database.size());
    for (const tuple<Vector3D, float, unsigned int> &star : stars_from_database) {
        const tuple<float,float> pixel_coor = ra_dec_to_pixel.convert_to_pixel_coordinates(get<0>(star), ZeroZeroPoint::upper_left);
        const float pos_x = get<0>(pixel_coor);
        const float pos_y = get<1>(pixel_coor);

        // skip stars outside of the sensor
        if (pos_x < 0 || pos_x > image_width_pixels || pos_y > 0 || -pos_y > image_height_pixels) continue;

        const float magnitude = get<1>(star);
        brightest_stars_from_database_pixel_coordinates.push_back(tuple<float,float,float>(pos_x, pos_y, magnitude));
    }

    // keep only 18 brightest stars from the photo (if available)
    std::vector<std::tuple<float,float,float> > brightest_stars_from_photo = stars_from_photo;
    if (brightest_stars_from_photo.size() > 18)   brightest_stars_from_photo.resize(18);
    const unsigned int n_stars_photo = brightest_stars_from_photo.size();

    // select 2 times more stars from database as we have from the photo (if available)
    if (brightest_stars_from_database_pixel_coordinates.size() > 2*n_stars_photo)   {
        brightest_stars_from_database_pixel_coordinates.resize(2*n_stars_photo);
    }

    unsigned int n_stars_truth_paired = 0;
    for (const auto &star_photo : brightest_stars_from_photo)  {
        bool paired_to_truth_star = false;
        for (const auto &star_truth : brightest_stars_from_database_pixel_coordinates)  {
            if (calculate_dist2(star_truth, star_photo) < maximal_allowed_deviation2)   {
                paired_to_truth_star = true;
            }
        }
        if (paired_to_truth_star) n_stars_truth_paired++;
    }

    return n_stars_truth_paired > 0.6*n_stars_photo;
};

vector<AsterismHashWithIndices> PlateSolverTool::get_hashes_with_indices(const vector<tuple<float,float,float> > &stars, unsigned nstars, unsigned int min_star4_index)   {
    vector<AsterismHashWithIndices> result;
    unsigned int star_indices[4];
    for (star_indices[0] = 0; star_indices[0] < nstars; star_indices[0]++) {
        for (star_indices[1] = (star_indices[0])+1; star_indices[1] < nstars; star_indices[1]++) {
            for (star_indices[2] = star_indices[1]+1; star_indices[2] < nstars; star_indices[2]++) {
                for (star_indices[3] = max(star_indices[2]+1, min_star4_index); star_indices[3] < nstars; star_indices[3]++) {
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

    // Get RA and dec of stars A, B
    const float RA_starA  = m_star_database_handler->get_star_ra(id_starA);
    const float dec_starA = m_star_database_handler->get_star_dec(id_starA);
    const float RA_starB  = m_star_database_handler->get_star_ra(id_starB);
    const float dec_starB = m_star_database_handler->get_star_dec(id_starB);

    const Vector3D starA_vector = Vector3D::get_vector_unity_from_ra_dec(RA_starA, dec_starA);
    const Vector3D starB_vector = Vector3D::get_vector_unity_from_ra_dec(RA_starB, dec_starB);

    // Calculate angle between the stars from A and B. Knowing this angle and pixel distance between the stars, calculate what is the angle covered by one pixel
    const float angle_A_B = Vector3D::get_angle(starA_vector, starB_vector);
    const float pixel_distance_A_B = sqrt( pow2(pos_x_starA - pos_x_starB) + pow2(pos_y_starA - pos_y_starB) );
    const float angle_per_pixel = angle_A_B/pixel_distance_A_B;

    // knowing angle per pixel, calculate angular width and height
    const float result_width_angle  = angle_per_pixel*image_width;
    const float result_height_angle = angle_per_pixel*image_height;

    // now we know angular width and height of the photo and coordinates of star A and B,
    // let's check what would be pixel coordinates of stars A and B if "up" direction of photo was pointing to north pole
    // we can later use this information to get the camera rotation
    RaDecToPixelCoordinatesConvertor ra_dec_to_pixel_convertor( RA_starA, dec_starA, 0,angle_per_pixel, m_image_width_pixels, m_image_height_pixels);
    vector<tuple<float,float> > pixel_coordinates(2);
    pixel_coordinates[0] = ra_dec_to_pixel_convertor.convert_to_pixel_coordinates(starA_vector, ZeroZeroPoint::center);
    pixel_coordinates[1] = ra_dec_to_pixel_convertor.convert_to_pixel_coordinates(starB_vector, ZeroZeroPoint::center);

    // calculate the came rotation (0 means "up" is towards north pole)
    const float rotation_around_starA = get_angle(      pos_x_starB - pos_x_starA, pos_y_starB - pos_y_starA,
                                                        get<0>(pixel_coordinates[1]), get<1>(pixel_coordinates[1]));

    // knowing camera rotation, RA and dec of star A and angular width/height, let's calculate RA and dec of the center of the photo
    PixelCoordinatesToRaDecConvertor convertor_center_in_starA(RA_starA, dec_starA, rotation_around_starA, angle_per_pixel, image_width, image_height);
    const tuple<float,float> RA_dec_center = convertor_center_in_starA.convert_to_ra_dec(
        image_width/2-pos_x_starA,      // coordinates of the center of the image with respect to starA
        -image_height/2-pos_y_starA,    // coordinates of the center of the image with respect to starA
        ZeroZeroPoint::center
    );

    const float RA_center  = get<0>(RA_dec_center);
    const float dec_center = get<1>(RA_dec_center);

    // now, finally we have to calculate the rotation again. For wide field photos, or positions close to north pole, the rotations around star A and around the center of image are quite different
    RaDecToPixelCoordinatesConvertor convertor_image_center_zero_rot(RA_center, dec_center, 0, angle_per_pixel, image_width, image_height);
    const tuple<float,float> starA_coordinates_rot0 = convertor_image_center_zero_rot.convert_to_pixel_coordinates(starA_vector, ZeroZeroPoint::center);
    const tuple<float,float> starA_coordinates_photo(-image_width/2+pos_x_starA,image_height/2+pos_y_starA);
    const float rotation = -get_angle(get<0>(starA_coordinates_rot0), get<1>(starA_coordinates_rot0),get<0>(starA_coordinates_photo), get<1>(starA_coordinates_photo));

    return tuple<float,float,float,float,float>(RA_center, dec_center, rotation, result_width_angle, result_height_angle);
};

float PlateSolverTool::calculate_dist2(const tuple<float,float,float> &star1, const tuple<float,float,float> &star2) {
    return pow2(get<0>(star1) - get<0>(star2)) + pow2(get<1>(star1) - get<1>(star2));
};