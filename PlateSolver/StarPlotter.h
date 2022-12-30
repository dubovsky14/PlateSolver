#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace PlateSolver   {
    class StarPlotter   {
        public:
            StarPlotter(unsigned int image_width, unsigned int image_height, unsigned char background_color = 0);

            void AddStarsFromPhoto(const std::vector<std::tuple<float,float,float> > &stars, unsigned char color = 255);

            void AddStarsFromDatabase(const std::vector<std::tuple<float,float,float> > &stars, unsigned char color = 255);

            void Save(const std::string &output_name);

        private:
            std::vector<unsigned char> m_pixels;

            unsigned int m_image_width, m_image_height;

            void draw_circle(float x, float y, float outer_radius, float inner_radius, unsigned char color = 255);

            inline void draw_pixel(unsigned int x, unsigned y, unsigned char color)    {
                const unsigned int pixel_index = -y*m_image_width + x;
                if (pixel_index >= m_pixels.size())    return;
                m_pixels[pixel_index] = color;
            };
    };
}