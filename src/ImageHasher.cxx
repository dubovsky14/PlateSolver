#include "../PlateSolver/ImageHasher.h"

#include "../PlateSolver/ImageLoader.h"
#include "../PlateSolver/AsterismHasher.h"
#include "../PlateSolver/StarFinder.h"

#include<vector>
#include<string>
#include<tuple>

using namespace std;
using namespace PlateSolver;

std::vector<std::tuple<float,float,float,float> >   PlateSolver::get_asterism_hashes_from_jpg(const string &jpg_address, unsigned number_of_stars_to_use)   {

    unsigned int image_width;
    std::vector<unsigned char> pixels = load_bw_image_to_uchar(jpg_address, &image_width);

    StarFinder star_finder(pixels, image_width);
    const float threshold = star_finder.get_threshold(0.002);

    std::vector<std::tuple<float, float, float> > stars = star_finder.get_stars(threshold);

    std::vector<std::tuple<float,float,float,float> > result;
    for (unsigned int i_star1 = 0; i_star1 < number_of_stars_to_use; i_star1++) {
        for (unsigned int i_star2 = i_star1+1; i_star2 < number_of_stars_to_use; i_star2++) {
            for (unsigned int i_star3 = i_star2+1; i_star3 < number_of_stars_to_use; i_star3++) {
                for (unsigned int i_star4 = i_star3+1; i_star4 < number_of_stars_to_use; i_star4++) {
                    if (i_star4 >= stars.size()) {
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
                    result.push_back(asterism_hash);
                }
            }
        }
    }
    return result;
};
