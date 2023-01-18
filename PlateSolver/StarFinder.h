#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <map>

#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>

namespace PlateSolver   {
    class StarFinder    {
        public:
            StarFinder(const std::string &photo_address);

            // returns vector of tuples<x-position, y-position, intensity>, threshold is the brightness threshold - pixels brighter than it will be considered to form stars
            std::vector<std::tuple<float, float, float> >   get_stars(float threshold, bool invert_y_axis = true);

            // get "histogram" from photography point of view -> return vector, where i-th element is count of the pixels with brightness "i"
            std::vector<unsigned int>   get_histogram() const {return m_histogram;};

            // find the brightness threshold, for which "part*m_pixels.size()" pixels are brighter than the threshold
            float   get_threshold(float part)   const;

            // 1st index = index of cluster, 2nd index = index of pixel, value = tuple<x_pixel, y_pixel>
            std::vector< std::vector<std::tuple<unsigned int, unsigned int> > > get_clusters(float threshold);

            unsigned int get_width()    const {return m_image.cols;};
            unsigned int get_height()   const {return m_image.rows;};

        private:
            cv::Mat                     m_image;

            unsigned int                m_height;
            unsigned int                m_width;
            inline unsigned char        read_pixel(unsigned int index_x, unsigned int index_y) const  {
                return m_image.at<uchar>(index_y, index_x);
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