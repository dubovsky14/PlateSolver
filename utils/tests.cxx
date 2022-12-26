#include "../PlateSolver/ImageLoader.h"
#include "../PlateSolver/StarFinder.h"
#include "../PlateSolver/AsterismHasher.h"
#include "../PlateSolver/StarDatabaseHandler.h"

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace PlateSolver;

int main(int argc, const char **argv)   {

    StarDatabaseHandler star_database_handler("../data/star_list.csv");
    float RA,dec,mag;
    string name;

    star_database_handler.get_star_info(24378, &RA, &dec, &mag, &name);
    cout << RA << "\t" << dec << "\t" << "\t" << mag << "\t" << name << endl;


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