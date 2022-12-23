#pragma once

#include <vector>
#include <tuple>
#include <map>

namespace PlateSolver   {
    class StarFinder    {
        public:
            StarFinder(const std::vector<unsigned char> &pixels, unsigned int pixels_per_line);

            // vector of tuples<x-position, y-position, intensity>
            std::vector<std::tuple<float, float, float> >   get_stars(float threshold);

            std::vector<unsigned int>   get_histogram() const {return m_histogram;};

            // find the brightness threshold, for which "part*m_pixels.size()" pixels are brighter than the threshold
            float   get_threshold(float part);

            // 1st index = index of cluster, 2nd index = index of pixel, value = tuple<x_pixel, y_pixel>
            std::vector< std::vector<std::tuple<unsigned int, unsigned int> > > get_clusters(float threshold);

        private:
            std::vector<unsigned char>  m_pixels;
            unsigned int                m_pixels_per_line;
            inline unsigned char        read_pixel(unsigned int index_x, unsigned int index_y) const  {
                return m_pixels[index_x + index_y*m_pixels_per_line];
            };

            void    reset_histogram();
            void    fill_histogram();
            std::vector<unsigned int> m_histogram;

            void fill_cluster(  unsigned int x_pos, unsigned int y_pos,
                                std::vector<std::tuple<unsigned int, unsigned int> > *current_cluster,
                                float threshold, std::map<std::tuple<unsigned int, unsigned int>, char> *visited_pixels);

            void calculate_center_of_cluster(   float  *x_pos, float  *y_pos,
                                                const std::vector<std::tuple<unsigned int, unsigned int> > &current_cluster);


    };
}