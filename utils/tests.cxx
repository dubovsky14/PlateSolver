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
        PlateSolverTool plate_solver_tool("../data/index_file_500mm_plane_approx.txt", "../data/catalogue.csv");


//        const tuple<float,float,float,float,float> result = plate_solver_tool.plate_solve(argv[1]);

//        const tuple<float,float,float,float,float> result = plate_solver_tool.get_hypothesis_coordinates(
//            5981.8, -2026.35, 3991,     //  "HD  23005"
//            1303.5,	-3140.96, 17617,    //  "V* MM Cam"
//            6240, 4160
//        ); // their distance is 4722 pixels

        const tuple<float,float,float,float,float> result = plate_solver_tool.get_hypothesis_coordinates(
            806,-711, 3991,     //  "HD  23005"
            346,-73, 17617,    //  "V* MM Cam"
            1369,852
        ); // their distance is 4722 pixels


        const float RA      = get<0>(result);
        const float dec     = get<1>(result);
        const float rot     = (180/M_PI)*get<2>(result);
        const float width   = (180/M_PI)*get<3>(result);
        const float height  = (180/M_PI)*get<4>(result);

        cout << "Plate solving finished\n";
        cout << "\tRA = " << convert_to_ged_min_sec(RA, "h") << endl;
        cout << "\tdec = " << convert_to_ged_min_sec(dec) << endl;
        cout << "\trot = " << convert_to_ged_min_sec(rot) << endl;
        cout << "\twidth x height = " << convert_to_ged_min_sec(width) + " x " << convert_to_ged_min_sec(height) << endl;

        return 0;
    }

    // hash finder test
    if (false)   {
        const string jpg_address = argv[1];
        StarDatabaseHandler star_database_handler("../data/catalogue.csv");

        vector<tuple<float,float,float,float> > hashes_from_photo = get_asterism_hashes_from_jpg(jpg_address, 8);

        HashFinder hash_finder("index_file_500mm_plane_approx.txt");

        const auto similar_hashes = hash_finder.get_similar_hashes(hashes_from_photo, 50);

        for (unsigned int i_input_hash = 0; i_input_hash < hashes_from_photo.size(); i_input_hash++)    {
            cout << "\n\nOriginal hash: " << hash_tuple_to_string(hashes_from_photo[i_input_hash]) << endl;
            for (const auto &similar_hash : similar_hashes[i_input_hash])    {
                float RA, dec;
                star_database_handler.get_star_info(get<1>(similar_hash), &RA, &dec);

                if (RA < 5 && RA > 3 && dec > 62 && dec < 80)   {
                    cout << hash_tuple_to_string(get<0>(similar_hash));
                    cout << " RA = " << RA << "   \tdec = " << dec << endl;
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
        night_sky_indexer->create_index_file("index_file_500mm_plane_approx.txt", 500);




        for (const auto &hash_info : hash_vector)   {
            const std::tuple<float,float,float,float> hash = get<0>(hash_info);
            const float xc = get<0>(hash);
            const float yc = get<1>(hash);
            const float xd = get<2>(hash);
            const float yd = get<3>(hash);

            unsigned int i_starA = get<1>(hash_info);
            unsigned int i_starB = get<2>(hash_info);
            unsigned int i_starC = get<3>(hash_info);
            unsigned int i_starD = get<4>(hash_info);

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