#include "../PlateSolver/StarPlotter.h"
#include "../PlateSolver/Common.h"

#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>

#include <string>
#include <tuple>
#include <vector>
#include <cmath>
#include <iostream>

using namespace PlateSolver;
using namespace std;

StarPlotter::StarPlotter(unsigned int image_width, unsigned int image_height, unsigned char background_color)   {
    m_image_height = image_height;
    m_image_width = image_width;
    m_pixels = vector<unsigned char>(m_image_width*m_image_height, background_color);
};

void StarPlotter::AddStarsFromPhoto(const std::vector<std::tuple<float,float,float> > &stars, unsigned char color)  {
    for (const std::tuple<float,float,float> &star : stars) {
        const float radius = sqrt(get<2>(star))/M_PI;
        draw_circle(get<0>(star), get<1>(star), radius*5, radius*3, color);
    }
};

void StarPlotter::AddStarsFromDatabase(const std::vector<std::tuple<float,float,float> > &stars, unsigned char color) {
    for (const std::tuple<float,float,float> &star : stars) {
        const float radius = 50*pow(0.5,get<2>(star)/3);
        draw_circle(get<0>(star), get<1>(star), radius*1.5, 0, color);
    }
};

void StarPlotter::Save(const std::string &output_name)  {
    dump_to_picture(output_name, m_pixels, m_image_width);
};

void StarPlotter::dump_to_picture(const std::string &image_address, const std::vector<unsigned char> &pixels, unsigned int pixels_per_line)    {
    int width(pixels_per_line), height(pixels.size()/pixels_per_line);
    cv::Mat image = cv::Mat::zeros(height, width,0);
    for (unsigned int i_pixel = 0; i_pixel < pixels.size(); i_pixel++)  {
        image.at<uchar>(i_pixel/width,i_pixel % width) = pixels[i_pixel];
    }
    cv::imwrite(image_address, image);
};

void StarPlotter::draw_circle(float x, float y, float outer_radius, float inner_radius, unsigned char color)  {
    const float outer_radius2 = pow2(outer_radius);
    const float inner_radius2 = pow2(inner_radius);
    for (int offset_x = x-outer_radius; offset_x <= x+outer_radius; offset_x++)   {
        for (int offset_y = y-outer_radius; offset_y <= y+outer_radius; offset_y++)   {
            const float r2 = pow2(offset_x-x) + pow2(offset_y-y);
            if (r2 < outer_radius2 && r2 > inner_radius2)  {
                draw_pixel(offset_x, offset_y, color);
            }
        }
    }
};
