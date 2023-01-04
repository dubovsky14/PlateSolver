#pragma once

#include <string>
#include <memory>
#include <vector>

namespace PlateSolver   {
    // return 1D vector of pixel values from 0-255 of gray-scale image and set pixels_per_line to the corresponding value.
    // 2D vector is not used because it's not continuous in memory
    std::vector<unsigned char> load_bw_image_to_uchar(const std::string &image_address, unsigned int *pixels_per_line);

    void dump_to_picture(const std::string &image_address, const std::vector<unsigned char> &pixels, unsigned int pixels_per_line);
}