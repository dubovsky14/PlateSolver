#pragma once

#include <vector>
#include <tuple>

namespace PlateSolver   {
    class StarFinder    {
        public:
            StarFinder(const std::vector<unsigned char> &pixels, unsigned int pixels_per_line);

            // vector of tuples<x-position, y-position, intensity>
            std::vector<std::tuple<float, float, float> >   get_stars();

            std::vector<unsigned int>   get_histogram() const {return m_histogram;};

            float   get_threshold(float part);

        private:
            std::vector<unsigned char>  m_pixels;
            unsigned int                m_pixels_per_line;
            inline unsigned char        read_pixel(unsigned int index_x, unsigned int index_y) const  {
                return m_pixels[index_x*m_pixels_per_line + index_y];
            };

            void    reset_histogram();
            void    fill_histogram();

            // find the brightness threshold, for which "part*m_pixels.size()" pixels are brighter than the threshold

            std::vector<unsigned int> m_histogram;

    };
}