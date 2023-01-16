#pragma once

#include "../PlateSolver/GeometricTransformations.h"
#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/StarPositionHandler.h"


#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>

#include <string>
#include <memory>

namespace PlateSolver   {
    class DescriptionAdder  {
        public:
            DescriptionAdder(cv::Mat *image);

            DescriptionAdder(cv::Mat *image, float ra_center, float dec_center, float camera_rotation, float angular_width);

            void add_star_description_ra_dec(float ra, float dec, const std::string &label);

            void add_star_description_pixel_coor(int xpos, int ypos, const std::string &label);

            void add_star_description(const StarDatabaseHandler &star_database_handler, unsigned int n_stars);

            void add_star_description(const StarDatabaseHandler &star_database_handler, const StarPositionHandler &star_position_handler, unsigned int n_stars);

            void save_image(const std::string &output_address);

        private:
            cv::Mat *m_image = nullptr;
            float m_font_size = 1;

            unsigned int m_width = 0;
            unsigned int m_height = 0;

            float m_ra_center   = 0;
            float m_dec_center  = 0;
            float m_angular_width = 0;
            float m_angular_height = 0;
            std::shared_ptr<RaDecToPixelCoordinatesConvertor> m_ra_dec_to_pix_convertor = nullptr;

            std::vector<std::tuple<float,float> > m_coordinates_of_added_descriptions;

            bool overlap_with_already_added(float x, float y);
    };
}