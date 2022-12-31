#include "../PlateSolver/ImageLoader.h"
#include "../PlateSolver/StarFinder.h"
#include "../PlateSolver/AsterismHasher.h"
#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/StarPositionHandler.h"
#include "../PlateSolver/NightSkyIndexer.h"
#include "../PlateSolver/HashFinder.h"
#include "../PlateSolver/Common.h"
#include "../PlateSolver/ImageHasher.h"
#include "../PlateSolver/PlateSolverTool.h"
#include "../PlateSolver/GeometricTransformations.h"
#include "../PlateSolver/StarPlotter.h"

#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include <memory>

using namespace std;
using namespace PlateSolver;

int main(int argc, const char **argv)   {
    try {

    if (true)   {

        unsigned int image_width_pixels;
        vector<unsigned char> m_pixels = load_bw_image_to_uchar(argv[1], &image_width_pixels);

        StarFinder star_finder(m_pixels, image_width_pixels);
        unsigned int image_height_pixels = m_pixels.size()/image_width_pixels;

        const float brightness_threshold = star_finder.get_threshold(0.002);
        vector<tuple<float,float,float> > stars_from_photo = star_finder.get_stars(brightness_threshold);
        if (stars_from_photo.size() > 10)  stars_from_photo.resize(10);


        StarDatabaseHandler star_database_handler("../data/catalogue.csv");
        StarPositionHandler star_position_handler(star_database_handler);
        std::vector<std::tuple<Vector3D, float, unsigned int> >  stars_around = star_position_handler.get_stars_around_coordinates(3.8172,68.1858, 0.025); // C5 galaxy

        while (stars_around.size() > 1.5*stars_from_photo.size())   {
            stars_around.erase(stars_around.begin() + stars_around.size());
        }

        PlateSolverTool plate_solver_tool("../data/index_file_840mm_plane_approx.txt", "../data/catalogue.csv");
        const tuple<float,float,float,float,float> hyp_coor = plate_solver_tool.get_hypothesis_coordinates(
            5981.8,	-2026.35, 3991,
            1303.5,	-3140.96, 17617,
            image_width_pixels, image_height_pixels
        );

        const bool valid_hypothesis = plate_solver_tool.validate_hypothesis(stars_from_photo, hyp_coor, image_width_pixels, image_height_pixels);
        cout << "Valid hypothesis: " << valid_hypothesis << endl;

        const float center_RA   = get<0>(hyp_coor);
        const float center_dec  = get<1>(hyp_coor);
        const float center_rot  = get<2>(hyp_coor);
        const float angle_per_pix  = get<3>(hyp_coor)/image_width_pixels;

        cout << "RA = " << center_RA << "\tdec = " << center_dec << "\t rot = " << (180/M_PI)*center_rot << endl;

        //return 0;

        //RaDecToPixelCoordinatesConvertor ra_dec_to_pixel_convertor( 3.806,68.428, 89.25*(M_PI/180),0.000006989, image_width_pixels, image_height_pixels);
        RaDecToPixelCoordinatesConvertor ra_dec_to_pixel_convertor( center_RA, center_dec, -center_rot, angle_per_pix, image_width_pixels, image_height_pixels);
        vector<tuple<float,float,float> > stars_from_database;
        for (const auto &star : stars_around)   {
            const tuple<float,float> ra_dec = ra_dec_to_pixel_convertor.convert_to_pixel_coordinates(get<0>(star));
            stars_from_database.push_back(tuple<float,float,float>(get<0>(ra_dec), get<1>(ra_dec), get<1>(star)));
        }
        const tuple<float,float> ra_dec = ra_dec_to_pixel_convertor.convert_to_pixel_coordinates(center_RA, center_dec);
        stars_from_database.push_back(tuple<float,float,float>(get<0>(ra_dec), get<1>(ra_dec), 0));

        PixelCoordinatesToRaDecConvertor pixel_to_ra_dec(center_RA, center_dec, center_rot, angle_per_pix, image_width_pixels, image_height_pixels);
        const tuple<float,float> should_be_center = pixel_to_ra_dec.convert_to_ra_dec(  get<0>(stars_from_database.back()),
                                                                                        get<1>(stars_from_database.back()));

        cout << "Converted vs. original difference: dRA = " << (get<0>(should_be_center))-center_RA <<
                                                    "\tdec = " << (get<1>(should_be_center))-center_dec << endl;

        StarPlotter star_plotter(image_width_pixels, image_height_pixels,255);
        star_plotter.AddStarsFromPhoto(stars_from_photo, 0);
        star_plotter.AddStarsFromDatabase(stars_from_database);
        star_plotter.Save("stars_test_horizontal.png");

        return 0;
    }

    if (true)   {
        PlateSolverTool plate_solver_tool("../data/index_file_840mm_plane_approx.txt", "../data/catalogue.csv");

        //const tuple<float,float,float,float,float> result = plate_solver_tool.get_hypothesis_coordinates(
        //    5981.8, -2026.35, 3991,     //  "HD  23005"
        //    1303.5,	-3140.96, 17617,    //  "V* MM Cam"
        //    6240, 4160
        //); // their distance is 4722 pixels


        const tuple<float,float,float,float,float> result = plate_solver_tool.plate_solve(argv[1]);
        const float RA      = get<0>(result);
        const float dec     = get<1>(result);
        const float rot     = (180/M_PI)*get<2>(result);
        const float width   = (180/M_PI)*get<3>(result);
        const float height  = (180/M_PI)*get<4>(result);

        cout << "Plate solving finished\n";
        cout << "\tRA = " << convert_to_deg_min_sec(RA, "h") << endl;
        cout << "\tdec = " << convert_to_deg_min_sec(dec) << endl;
        cout << "\trot = " << convert_to_deg_min_sec(rot) << endl;
        cout << "\twidth x height = " << convert_to_deg_min_sec(width) + " x " << convert_to_deg_min_sec(height) << endl;

        return 0;
    }

    // hash finder test
    if (true)   {
        const string jpg_address = argv[1];
        StarDatabaseHandler star_database_handler("../data/catalogue.csv");

        unsigned int image_width_pixels;
        vector<unsigned char> m_pixels = load_bw_image_to_uchar(jpg_address, &image_width_pixels);

        StarFinder star_finder(m_pixels, image_width_pixels);
        unsigned int image_height_pixels = m_pixels.size()/image_width_pixels;

        const float brightness_threshold = star_finder.get_threshold(0.002);
        vector<tuple<float,float,float> > stars = star_finder.get_stars(brightness_threshold);

        const vector<AsterismHashWithIndices> hashes_with_indices_from_photo = PlateSolverTool::get_hashes_with_indices(stars, 8);
        const vector<AsterismHash>  hashes_from_photo = extract_hashes(hashes_with_indices_from_photo);

        HashFinder hash_finder("../data/index_file_840mm_plane_approx.txt");
        PlateSolverTool plate_solver_tool("../data/index_file_840mm_plane_approx.txt", "../data/catalogue.csv");

        const auto similar_hashes = hash_finder.get_similar_hashes(hashes_from_photo, 50);

        for (unsigned int i_input_hash = 0; i_input_hash < hashes_from_photo.size(); i_input_hash++)    {
            cout << "\n\nOriginal hash: " << hash_tuple_to_string(hashes_from_photo[i_input_hash]);
            cout << ",\t(" << get<0>(stars[get<1>(hashes_with_indices_from_photo[i_input_hash])]) << ", " << get<1>(stars[get<1>(hashes_with_indices_from_photo[i_input_hash])]) << ") ";
            cout << ",\t(" << get<0>(stars[get<2>(hashes_with_indices_from_photo[i_input_hash])]) << ", " << get<1>(stars[get<2>(hashes_with_indices_from_photo[i_input_hash])]) << ") ";
            cout << ",\t(" << get<0>(stars[get<3>(hashes_with_indices_from_photo[i_input_hash])]) << ", " << get<1>(stars[get<3>(hashes_with_indices_from_photo[i_input_hash])]) << ") ";
            cout << ",\t(" << get<0>(stars[get<4>(hashes_with_indices_from_photo[i_input_hash])]) << ", " << get<1>(stars[get<4>(hashes_with_indices_from_photo[i_input_hash])]) << ")\n";
            for (const auto &similar_hash : similar_hashes[i_input_hash])    {
                float RA, dec;
                star_database_handler.get_star_info(get<1>(similar_hash), &RA, &dec);

                const string starA_name = star_database_handler.get_star_name(get<1>(similar_hash));
                const string starB_name = star_database_handler.get_star_name(get<2>(similar_hash));
                const string starC_name = star_database_handler.get_star_name(get<3>(similar_hash));
                const string starD_name = star_database_handler.get_star_name(get<4>(similar_hash));

                if (RA < 5 && RA > 3 && dec > 62 && dec < 80)   {
                    const unsigned int i_photo_starA = get<1>(hashes_with_indices_from_photo[i_input_hash]);
                    const unsigned int i_photo_starB = get<2>(hashes_with_indices_from_photo[i_input_hash]);
                    const auto hyp_coor = plate_solver_tool.get_hypothesis_coordinates(
                        get<0>(stars[i_photo_starA]),
                        get<1>(stars[i_photo_starA]),
                        get<1>(similar_hash),
                        get<0>(stars[i_photo_starB]),
                        get<1>(stars[i_photo_starB]),
                        get<2>(similar_hash),
                        6240,4160
                    );
                    cout << hash_tuple_to_string(get<0>(similar_hash));
                    cout << " RA = " << get<0>(hyp_coor) << "   \tdec = " << get<1>(hyp_coor) << "\t\t"
                            << starA_name << "\t" << starB_name << "\t" << starC_name << "\t" << starD_name  <<  endl;
                }
            }
        }
        return 0;
    }

    if (false)  {
        StarDatabaseHandler star_database_handler("../data/catalogue.csv");
        float RA,dec,mag;
        string name;

        shared_ptr<StarPositionHandler> star_position_handler = make_shared<StarPositionHandler>(star_database_handler);
        //std::vector<std::tuple<Vector3D, float, unsigned int> >  stars_around = star_position_handler->get_stars_around_coordinates(5.698,-1.92, 0.0085); // Alnitak
        //std::vector<std::tuple<Vector3D, float, unsigned int> >  stars_around = star_position_handler->get_stars_around_coordinates(12.92,55.89, 0.01); // Allioth
        std::vector<std::tuple<Vector3D, float, unsigned int> >  stars_around = star_position_handler->get_stars_around_coordinates(3.8172,68.1858, 0.025); // C5 galaxy

        const Vector3D reference_axis(1, -1.92*(M_PI/180), 5.698*(M_PI/12), CoordinateSystem::enum_spherical);

        shared_ptr<NightSkyIndexer> night_sky_indexer = make_shared<NightSkyIndexer>(star_position_handler);
        std::vector<std::tuple<std::tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > hash_vector;
        night_sky_indexer->index_sky_region(3.8172,68.1858, 0.025, &hash_vector);
        night_sky_indexer->create_index_file("index_file_840mm_plane_approx.txt", 840);




        for (const auto &hash_info : hash_vector)   {
            const std::tuple<float,float,float,float> hash = get<0>(hash_info);
            const float xc = get<0>(hash);
            const float yc = get<1>(hash);
            const float xd = get<2>(hash);
            const float yd = get<3>(hash);

            const unsigned int i_starA = get<1>(hash_info);
            const unsigned int i_starB = get<2>(hash_info);
            const unsigned int i_starC = get<3>(hash_info);
            const unsigned int i_starD = get<4>(hash_info);

            vector<Vector3D> star_positions;
            for (unsigned int i_star : vector<unsigned int>({i_starA,i_starB,i_starC,i_starD,}))    {
                star_database_handler.get_star_info(i_star, &RA, &dec);
                star_positions.push_back(Vector3D(1, dec*(M_PI/180), RA*(-M_PI/12), CoordinateSystem::enum_spherical));
            }
            vector<tuple<float,float> > star_positions_in_sensor_coordinates = NightSkyIndexer::convert_star_coordinates_to_pixels_positions(star_positions, reference_axis);

            const std::string nameA = star_database_handler.get_star_name(i_starA);
            const std::string nameB = star_database_handler.get_star_name(i_starB);
            const std::string nameC = star_database_handler.get_star_name(i_starC);
            const std::string nameD = star_database_handler.get_star_name(i_starD);

            cout << "[" << xc << ", " << yc << ", " << xd << ", " << yd << "], "
                << nameA << "" << " [ " << 1000*get<0>(star_positions_in_sensor_coordinates[0])-238.568 << ", " << 1000*get<1>(star_positions_in_sensor_coordinates[0])-910.618  << " ], "
                << nameB << "" << " [ " << 1000*get<0>(star_positions_in_sensor_coordinates[1])-238.568 << ", " << 1000*get<1>(star_positions_in_sensor_coordinates[1])-910.618  << " ], "
                << nameC << "" << " [ " << 1000*get<0>(star_positions_in_sensor_coordinates[2])-238.568 << ", " << 1000*get<1>(star_positions_in_sensor_coordinates[2])-910.618  << " ], "
                << nameD << "" << " [ " << 1000*get<0>(star_positions_in_sensor_coordinates[3])-238.568 << ", " << 1000*get<1>(star_positions_in_sensor_coordinates[3])-910.618  << " ]\n";
        }


        return 0;
    }

    if (true)   {
        unsigned int pixels_per_line;
        vector<unsigned char> pixels = load_bw_image_to_uchar(argv[1], &pixels_per_line);
        StarFinder star_finder(pixels, pixels_per_line);

        const float threshold = star_finder.get_threshold(0.002);
        cout << endl << "threshold(0.05%) = " << threshold << endl;

        vector<unsigned char> pixels_above_threshold(pixels.size());
        for (unsigned int i = 0; i < pixels.size(); i++)  {
            pixels_above_threshold[i] = 240*(pixels[i] > threshold);
        }
        dump_to_picture("stars.png", pixels_above_threshold, pixels_per_line);

        std::vector<std::tuple<float, float, float> > stars = star_finder.get_stars(threshold);

        cout << "Number of found stars: " << stars.size() << endl;
        for (const std::tuple<float, float, float> &star : stars)    {
            const float x_pos       = get<0>(star);
            const float y_pos       = get<1>(star);
            const float brightness  = get<2>(star);
            cout << x_pos << "\t" << y_pos << "\t" << brightness << endl;
        }

        /*
        //  0,3,1,4
        tuple<float,float,float,float> asterism_hash;
        vector<tuple<float, float> > stars_to_hash = {
            tuple(get<0>(stars[0]), get<1>(stars[0])),
            tuple(get<0>(stars[3]), get<1>(stars[3])),
            tuple(get<0>(stars[1]), get<1>(stars[1])),
            tuple(get<0>(stars[4]), get<1>(stars[4])),
        };
        unsigned int starA,starB,starC,starD;
        const bool valid_hash = calculate_asterism_hash(stars_to_hash, &asterism_hash,&starA,&starB,&starC,&starD);
        cout << "\n\nStars to hash:\n";
        cout << get<0>(stars_to_hash[starA]) << "\t" << get<1>(stars_to_hash[starA]) << endl;
        cout << get<0>(stars_to_hash[starB]) << "\t" << get<1>(stars_to_hash[starB]) << endl;
        cout << get<0>(stars_to_hash[starC]) << "\t" << get<1>(stars_to_hash[starC]) << endl;
        cout << get<0>(stars_to_hash[starD]) << "\t" << get<1>(stars_to_hash[starD]) << endl;
        cout << "Hash:\t"   << get<0>(asterism_hash) << ", "
                            << get<1>(asterism_hash) << ", "
                            << get<2>(asterism_hash) << ", "
                            << get<3>(asterism_hash) << endl;
        */


        const unsigned int NSTARS = 7;
        for (unsigned int i_star1 = 0; i_star1 < NSTARS; i_star1++) {
            for (unsigned int i_star2 = i_star1+1; i_star2 < NSTARS; i_star2++) {
                for (unsigned int i_star3 = i_star2+1; i_star3 < NSTARS; i_star3++) {
                    for (unsigned int i_star4 = i_star3+1; i_star4 < NSTARS; i_star4++) {
                        tuple<float,float,float,float> asterism_hash;
                        vector<tuple<float, float> > stars_to_hash = {
                            tuple(get<0>(stars[i_star1]), get<1>(stars[i_star1])),
                            tuple(get<0>(stars[i_star2]), get<1>(stars[i_star2])),
                            tuple(get<0>(stars[i_star3]), get<1>(stars[i_star3])),
                            tuple(get<0>(stars[i_star4]), get<1>(stars[i_star4])),
                        };
                        unsigned int starA,starB,starC,starD;
                        const bool valid_hash = calculate_asterism_hash(stars_to_hash, &asterism_hash,&starA,&starB,&starC,&starD);
                        cout << "\n\nStars to hash:\n";
                        cout << get<0>(stars_to_hash[starA]) << "\t" << get<1>(stars_to_hash[starA]) << endl;
                        cout << get<0>(stars_to_hash[starB]) << "\t" << get<1>(stars_to_hash[starB]) << endl;
                        cout << get<0>(stars_to_hash[starC]) << "\t" << get<1>(stars_to_hash[starC]) << endl;
                        cout << get<0>(stars_to_hash[starD]) << "\t" << get<1>(stars_to_hash[starD]) << endl;

                        if (!valid_hash)    {
                            cout << "Invalid hash\n";
                            continue;
                        }
                        cout << "Hash:\t"   << get<0>(asterism_hash) << ", "
                                            << get<1>(asterism_hash) << ", "
                                            << get<2>(asterism_hash) << ", "
                                            << get<3>(asterism_hash) << endl;

                    }
                }
            }
        }
    }

    }
    catch(const string &e)  {
        cout << e << endl;
        abort();
    }

/*
    std::vector< std::vector<std::tuple<unsigned int, unsigned int> > > clusters = star_finder.get_clusters(threshold);
    sort(clusters.begin(), clusters.end(), [](const auto &a, const auto &b) {return a.size() > b.size();}  );
    cout << "Number of found clusters: " << clusters.size() << endl;
    for (const std::vector<std::tuple<unsigned int, unsigned int> > &cluster : clusters)    {
        const unsigned int cluster_size = cluster.size();
        const unsigned int x_pos = get<0>(cluster[0]);
        const unsigned int y_pos = get<1>(cluster[0]);
        cout << x_pos << "\t" << y_pos << "\t" << cluster_size << endl;
    }
*/


}