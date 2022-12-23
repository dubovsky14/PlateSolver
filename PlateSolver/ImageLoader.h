#pragma once

#include <string>
#include <memory>
#include <vector>

namespace PlateSolver   {
    std::vector<unsigned char> load_bw_image_to_uchar(const std::string &image_address, unsigned int *pixels_per_line);

    void dump_to_picture(const std::string &image_address, const std::vector<unsigned char> &pixels, unsigned int pixels_per_line);
}