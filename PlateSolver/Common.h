#pragma once

#include<string>
#include<tuple>
#include<vector>

namespace PlateSolver   {
    typedef std::tuple<float,float,float,float> AsterismHash;

    typedef std::tuple<std::tuple<float,float,float,float>, unsigned int, unsigned int, unsigned int, unsigned int> AsterismHashWithIndices;

    std::string convert_to_ged_min_sec(float x);

    std::string hash_tuple_to_string(std::tuple<float,float,float,float> hash);

    std::vector<AsterismHash> extract_hashes(const std::vector<AsterismHashWithIndices> &asterisms_and_hashes);

    template <class X>
    X pow2(X x) {return x*x;};

    // if vec2 can be obtained by anti-clock wise rotation of vec1 by (0,pi), the result is positive, otherwise negative
    float get_angle(float vec1_x, float vec1_y, float vec2_x, float vec2_y);


}