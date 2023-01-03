// for more details of the algorithm see: https://arxiv.org/pdf/0910.2233.pdf
#pragma once

#include <tuple>
#include <vector>

namespace PlateSolver   {
    bool calculate_asterism_hash(   const std::vector<std::tuple<float, float > > &stars, std::tuple<float,float,float,float> *result,
                                    unsigned int *index_star_A = nullptr,
                                    unsigned int *index_star_B = nullptr,
                                    unsigned int *index_star_C = nullptr,
                                    unsigned int *index_star_D = nullptr);

    void get_indices_of_most_distant_stars(const std::vector<std::tuple<float, float > > &stars, int *star1, int *star2);

    float get_star_distance_squared(const std::tuple<float, float> &star1, const std::tuple<float, float> &star2);

    inline float calculate_coordinate_along_axis(const float *original_vector, const float *axis)   {
        return (original_vector[0]*axis[0] + original_vector[1]*axis[1]);
    };
}