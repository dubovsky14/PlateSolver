#include "../PlateSolver/ImageLoader.h"
#include "../PlateSolver/StarFinder.h"
#include "../PlateSolver/AsterismHasher.h"
#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/StarPositionHandler.h"
#include "../PlateSolver/NightSkyIndexer.h"
#include "../PlateSolver/HashFinder.h"
#include "../PlateSolver/Common.h"
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

    if (false)   {

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
        unsigned int pixels_per_line;
        vector<unsigned char> pixels = load_bw_image_to_uchar(argv[1], &pixels_per_line);
        StarFinder star_finder(pixels, pixels_per_line);

        const float threshold = star_finder.get_threshold(0.0005);
        cout << endl << "threshold(0.05%) = " << threshold << endl;

        vector<unsigned char> pixels_above_threshold(pixels.size());
        for (unsigned int i = 0; i < pixels.size(); i++)  {
            pixels_above_threshold[i] = 255*(pixels[i] > threshold);
        }
        dump_to_picture("stars.png", pixels_above_threshold, pixels_per_line);

        std::vector<std::tuple<float, float, float> > stars = star_finder.get_stars(threshold);

        //cout << get<0>(stars[0]) << ", " << get<1>(stars[0]) << endl;
        //cout << get<0>(stars[1]) << ", " << get<1>(stars[1]) << endl;
        if (stars.size() > 15)  stars.resize(15);
        const unsigned int sensor_x(6240), sensor_y(4160);

        //StarDatabaseHandler star_database("../data/star_list.csv");
        StarDatabaseHandler star_database("../data/catalogue.csv");
        StarPositionHandler star_position_handler(star_database);
        //auto truth_stars_around1 = star_position_handler.get_stars_around_coordinates(14.046198273, 54.25255, 0.004);
        //const unsigned int id1 = get<2>(truth_stars_around1[0]);
        //cout << star_database.get_star_name(id1);


        PlateSolverTool plate_solver_tool("../data/index_file_840mm.bin", "../data/catalogue.csv");
        const tuple<float,float,float,float,float> hyp_coor = plate_solver_tool.get_hypothesis_coordinates( // M101 photo
            5858.45, -598.438, 13823,   // HD 122200
            5492, -3480.06, 17772,      // HD 123518
            sensor_x, sensor_y
        );

        //  287155,"HD 122200B",13.98207081090133,53.10867143174,9.76
        //  17772,"HD 123518",14.111463321051332,53.312304181730006,7.12

        const float RA = get<0>(hyp_coor);
        const float dec = get<1>(hyp_coor);
        const float rot = get<2>(hyp_coor);
        const float rad_per_pix = get<3>(hyp_coor)/6240;
        const float width   = (180/M_PI)*get<3>(hyp_coor);
        const float height  = (180/M_PI)*get<4>(hyp_coor);


        cout << "\tRA = " << convert_to_deg_min_sec(RA, "h") << endl;
        cout << "\tdec = " << convert_to_deg_min_sec(dec) << endl;
        cout << "\trot = " << convert_to_deg_min_sec(rot) << endl;
        cout << "\twidth x height = " << convert_to_deg_min_sec(width) + " x " << convert_to_deg_min_sec(height) << endl;

        auto truth_stars_around = star_position_handler.get_stars_around_coordinates(RA, dec, 0.03);
        RaDecToPixelCoordinatesConvertor ra_dec_to_pix(RA, dec, -rot, rad_per_pix, sensor_x, sensor_y);
        vector<tuple<float,float,float> > pixel_coordinates_truth_stars;

        for (const auto &truth_star : truth_stars_around)   {
            const Vector3D star_position = get<0>(truth_star);
            const float magnitude       = get<1>(truth_star);

            const tuple<float,float> pix_coordinates = ra_dec_to_pix.convert_to_pixel_coordinates(star_position);
            const float x = get<0>(pix_coordinates);
            const float y = get<1>(pix_coordinates);
            if (x < 0 || x > sensor_x || y > 0 || -y > sensor_y)    {
                continue;
            }

            cout << get<2>(truth_star) << "\t" << star_database.get_star_name(get<2>(truth_star)) << "\t" << magnitude << "\t[ " << x << ", " << y << " ]" << endl;
            pixel_coordinates_truth_stars.push_back(tuple<float,float,float>(x,y, magnitude));

        }

        //const Vector3D reference_axis = Vector3D::get_vector_unity_from_ra_dec(RA, dec);
        const Vector3D reference_axis = Vector3D::get_vector_unity_from_ra_dec(14.111463321051332,53.312304181730006);
        const tuple<float,float> should_be_center = ra_dec_to_pix.convert_to_pixel_coordinates(reference_axis);
        pixel_coordinates_truth_stars.push_back(tuple<float,float,float>(
            get<0>(should_be_center), get<1>(should_be_center), 2
        ));

        //if (pixel_coordinates_truth_stars.size() > 20)  pixel_coordinates_truth_stars.resize(20);

        StarPlotter star_plotter(6240, 4160, 255);
        star_plotter.AddStarsFromPhoto(stars, 0);
        star_plotter.AddStarsFromDatabase(pixel_coordinates_truth_stars);
        star_plotter.Save("stars_photo.png");

    }





    // plot star pixels
    if (false)   {
        unsigned int pixels_per_line;
        vector<unsigned char> pixels = load_bw_image_to_uchar(argv[1], &pixels_per_line);
        StarFinder star_finder(pixels, pixels_per_line);

        const float threshold = star_finder.get_threshold(0.0005);
        cout << endl << "threshold(0.05%) = " << threshold << endl;

        vector<unsigned char> pixels_above_threshold(pixels.size());
        for (unsigned int i = 0; i < pixels.size(); i++)  {
            pixels_above_threshold[i] = 255*(pixels[i] > threshold);
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