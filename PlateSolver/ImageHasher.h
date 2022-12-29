#pragma once

#include "../PlateSolver/ImageLoader.h"
#include "../PlateSolver/AsterismHasher.h"

#include<vector>
#include<string>
#include<tuple>


namespace PlateSolver   {
    std::vector<std::tuple<float,float,float,float> >   get_asterism_hashes_from_jpg(const std::string &jpg_address, unsigned number_of_stars_to_use);
}