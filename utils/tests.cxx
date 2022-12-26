#include "../PlateSolver/ImageLoader.h"
#include "../PlateSolver/StarFinder.h"
#include "../PlateSolver/AsterismHasher.h"
#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/StarPositionHandler.h"

#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace PlateSolver;

int main(int argc, const char **argv)   {

    StarDatabaseHandler star_database_handler("../data/star_list.csv");
    float RA,dec,mag;
    string name;

    StarPositionHandler star_position_handler(star_database_handler);
    std::vector<std::tuple<Vector3D, float, unsigned int> >  stars_around = star_position_handler.get_stars_around_coordinates(5.698,-1.92, 0.0085); // Alnitak
    //std::vector<std::tuple<Vector3D, float, unsigned int> >  stars_around = star_position_handler.get_stars_around_coordinates(12.92,55.89, 0.01); // Allioth

    const Vector3D reference_axis(1, -1.92*(M_PI/180), 5.698*(M_PI/12), CoordinateSystem::enum_spherical);
    cout << "Number of stars nearby: " << stars_around.size() << endl;
    for (const auto &star : stars_around)    {
        const unsigned int id = get<2>(star);
        const Vector3D star_position = get<0>(star);
        star_database_handler.get_star_info(id, &RA, &dec, &mag, &name);
        cout << RA << "  \t" << dec << "  \t" << mag << "\t" << name << endl;
        //cout << star_position.get_ra() << "  \t" << star_position.get_dec() << "  \t" << get<0>(star).scalar_product(reference_axis) << "\t" << name << endl;
        //cout << star_position.x() << "  \t" << star_position.y() << "  \t" << star_position.z() << "  \t";
        //cout << star_position.get_ra() << "  \t" << star_position.get_dec() << "  \t" << (reference_axis*(get<0>(star))).r2() << "\t" << name << endl;
        //cout << star_position.x() << "  \t" << star_position.y() << "  \t" << star_position.z() << "\t" << name << endl;

    }



    return 0;

    unsigned int pixels_per_line;
    vector<unsigned char> pixels = load_bw_image_to_uchar(argv[1], &pixels_per_line);
    StarFinder star_finder(pixels, pixels_per_line);
    vector<unsigned int> histogram = star_finder.get_histogram();

    for (unsigned int i = 0; i < histogram.size(); i++) {
        cout << i << "\t" << histogram[i] << endl;
    }

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
                    const bool valid_hash = calculate_asterism_hash(stars_to_hash, &asterism_hash);
                    cout << "\n\nStars to hash:\n";
                    for (const tuple<float,float> &star : stars_to_hash)    {
                        cout << get<0>(star) << "\t" << get<1>(star) << endl;
                    }

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