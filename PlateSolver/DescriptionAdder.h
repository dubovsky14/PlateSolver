#pragma once

#include "../PlateSolver/GeometricTransformations.h"
#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/StarPositionHandler.h"


#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>

#include <string>
#include <memory>

namespace PlateSolver   {
    /**
     * @brief Class responsible for adding labels of stars and deep sky objects to a jpg file
     *
     */
    class DescriptionAdder  {
        public:
            DescriptionAdder(cv::Mat *image);

            /**
             * @brief Construct a new Description Adder object
             *
             * @param image - input image as cv::Mat
             * @param ra_center - right ascension coordinate of the center of the image
             * @param dec_center - declination coordinate of the center of the image
             * @param camera_rotation - angle in radians between the "up" vector and direction to north celestial pole
             * @param angular_width - angle (in radians) covered by photo in x-axis (width) direction
             */
            DescriptionAdder(cv::Mat *image, float ra_center, float dec_center, float camera_rotation, float angular_width);

            /**
             * @brief Add label for the star at given coordinates. Do not add it if it overlaps with an already added label
             *
             * @param ra Right ascension of the star
             * @param dec Declination of the star
             * @param label label shown in the picture
             */
            void add_star_description_ra_dec(float ra, float dec, const std::string &label);

            /**
             * @brief Add label for the star at given coordinates. Do not add it if it overlaps with an already added label
             *
             * @param xpos x-pixel coordinate of the star
             * @param ypos y-pixel coordinate of the star. 0 is top of the image, -1400 is 1400 pixels under the top of the image
             * @param label label shown in the picture
             */
            void add_star_description_pixel_coor(int xpos, int ypos, const std::string &label);

            /**
             * @brief Add labels for a given number of the brightest stars from the database
             *
             * @param star_database_handler
             * @param n_stars - number of stars for which the labels should be added
             */
            void add_star_description(const StarDatabaseHandler &star_database_handler, unsigned int n_stars);

            /**
             * @brief Add labels for a given number of the brightest stars from the database
             *
             * @param star_database_handler
             * @param star_position_handler
             * @param n_stars - number of stars for which the labels should be added
             */
            void add_star_description(const StarDatabaseHandler &star_database_handler, const StarPositionHandler &star_position_handler, unsigned int n_stars);

            /**
             * @brief Save image to the output (jpg) file
             */
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

            /**
             * @brief Does the label overlap with an already added label?
             *
             * @param x - x coordinate of the label in pixels
             * @param y - y coordinate of the label in pixels, 0 is top edge of the image, all points in the image has negative y coordinate
             * @return true - overlaps
             * @return false - does not overlap
             */
            bool overlap_with_already_added(float x, float y);
    };
}