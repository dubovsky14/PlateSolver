#pragma once

#include<string>
#include<tuple>
#include<vector>

namespace PlateSolver   {
    std::string convert_to_ged_min_sec(float x);

    std::string hash_tuple_to_string(std::tuple<float,float,float,float> hash);
}