#include "../PlateSolver/ImageLoader.h"
#include "../PlateSolver/StarFinder.h"

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace PlateSolver;

int main(int argc, const char **argv)   {
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