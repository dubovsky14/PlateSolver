#pragma once

#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/HashFinder.h"
#include "../PlateSolver/Common.h"
#include "../PlateSolver/NightSkyIndexer.h"
#include "../PlateSolver/StarPositionHandler.h"
#include "../PlateSolver/StarFinder.h"


#include<vector>
#include<string>
#include<tuple>
#include<memory>

#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>

namespace   PlateSolver {
    class PlateSolverTool   {
        public:
            PlateSolverTool() = delete;

            /**
             * @brief Construct a new Plate Solver Tool object
             *
             * @param stars_catalogue address of the star catalogue - either .csv or .bin file
             */
            PlateSolverTool(const std::string &stars_catalogue);


            /**
             * @brief Construct a new Plate Solver Tool object
             *
             * @param star_database_handler - shared pointer to the star database handler
             * @param star_position_handler - shared pointer to the star position handler
             */
            PlateSolverTool(std::shared_ptr<StarDatabaseHandler> star_database_handler, std::shared_ptr<StarPositionHandler> star_position_handler);

            /**
             * @brief Set the hash file object
             *
             * @param hash_file address of the file with asterism hashes: 3 types are supported: .txt (.csv), .bin or .kdtree
            */
            void set_hash_file(const std::string &hash_file);

            /**
             * @brief Calculate celestial coordinates of the center of the photo
             *
             * @param jpg_file - address of the jpg file to plate solve
             * @param hash_file address of the file with asterism hashes: 3 types are supported: .txt (.csv), .bin or .kdtree
             * @return std::tuple<float,float,float,float,float> :[RA, dec, rotation (zero means upwards is towards the north celestial pole), width in radians, height in radians]
             */
            std::tuple<float,float,float,float,float> plate_solve(const std::string &jpg_file, const std::string &hash_file);

            /**
             * @brief Calculate celestial coordinates of the center of the photo
             *
             * @param photo - gray scale image as cv::Mat object
             * @param hash_file address of the file with asterism hashes: 3 types are supported: .txt (.csv), .bin or .kdtree
             * @return std::tuple<float,float,float,float,float> :[RA, dec, rotation (zero means upwards is towards the north celestial pole), width in radians, height in radians]
             */
            std::tuple<float,float,float,float,float> plate_solve(const cv::Mat &photo, const std::string &hash_file);

            /**
             * @brief Get coordinates of the picture if the hypothesis is correct
             *
             * @param pos_x_starA   x coordinate of the starA from the photo
             * @param pos_y_starA   y coordinate of the starA from the photo
             * @param id_starA      id (from the catalogue file) of the starA
             * @param pos_x_starB   x coordinate of the starb from the photo
             * @param pos_y_starB   y coordinate of the starb from the photo
             * @param id_starB      id (from the catalogue file) of the starb
             * @param image_width   width of the photo in pixels
             * @param image_height  height of the photo in pixels
             * @return std::tuple<float,float,float,float,float> - RA [hours], dec [degrees], camera rotation [rad], angular width [rad], angular height [rad]
             */
            std::tuple<float,float,float,float,float> get_hypothesis_coordinates(   float pos_x_starA, float pos_y_starA, unsigned int id_starA,
                                                                                    float pos_x_starB, float pos_y_starB, unsigned int id_starB,
                                                                                    float image_width, float image_height);

            /**
             * @brief Return vector of asterism hashes and indices of the stars from photo
             *
             * @param stars - vector of tuples <x pixel position, y pixel position, magnitude>
             * @param nstars - number of stars to consider
             * @param min_star4_index - minimal index of star 4 - can be used to get for example when plate solving for 8 brightest stars fails and you have to calculate the missing hashes using 10 stars (not calculating again for example hash of stars 1,2,3,4)
             * @return std::vector<AsterismHashWithIndices> - vector of asterism hashes and the corresponding stars indices
             */
            static std::vector<AsterismHashWithIndices> get_hashes_with_indices(const std::vector<std::tuple<float,float,float> > &stars, unsigned nstars, unsigned int min_star4_index = 0);

            /**
             * @brief Return true if hypothesis is correct
             *
             * @param stars_from_photo pixel coordinates and magnitudes of the stars from the photo
             * @param hypothesis_coordinates [RA, dec, rotation, width in radians, height in radians]
             * @param image_width_pixels
             * @param image_height_pixels
             * @return true hypothesis is valid
             * @return false hypothesis is not valid
             */
            bool validate_hypothesis(   const std::vector<std::tuple<float,float,float> > &stars_from_photo,
                                        const std::tuple<float,float,float,float,float> &hypothesis_coordinates,
                                        float image_width_pixels, float image_height_pixels);


            static float calculate_dist2(const std::tuple<float,float,float> &star1, const std::tuple<float,float,float> &star2);

        private:

            std::shared_ptr<StarDatabaseHandler>    m_star_database_handler = nullptr;
            std::shared_ptr<StarPositionHandler>    m_star_position_handler = nullptr;
            std::unique_ptr<HashFinder>             m_hash_finder           = nullptr;
            std::unique_ptr<NightSkyIndexer>        m_night_sky_indexer     = nullptr;


            std::vector<unsigned char> m_pixels;
            unsigned int m_image_width_pixels = 0;
            unsigned int m_image_height_pixels= 0;

            std::tuple<float,float,float,float,float> plate_solve(const StarFinder &star_finder);
    };
}