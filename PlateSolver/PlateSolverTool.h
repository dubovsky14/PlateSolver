#pragma once

#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/HashFinder.h"
#include "../PlateSolver/Common.h"

#include<vector>
#include<string>
#include<tuple>
#include<memory>

namespace   PlateSolver {
    class PlateSolverTool   {
        public:
            PlateSolverTool(const std::string &hash_file, const std::string &stars_catalogue);

            // returns : [RA, dec, rotation (zero means upwards is towards the north celestial pole), width in radians, height in radians]
            std::tuple<float,float,float,float,float> plate_solve(const std::string &jpg_file);

            // returns : [RA, dec, rotation (zero means upwards is towards the north celestial pole), width in radians, height in radians]
            std::tuple<float,float,float,float,float> get_hypothesis_coordinates(   float pos_x_starA, float pos_y_starA, unsigned int id_starA,
                                                                                    float pos_x_starB, float pos_y_starB, unsigned int id_starB,
                                                                                    float image_width, float image_height);

            std::vector<AsterismHashWithIndices> get_hashes_with_indices(const std::vector<std::tuple<float,float,float> > &stars, unsigned nstars);

        private:

            std::shared_ptr<StarDatabaseHandler>    m_star_database_handler = nullptr;
            std::shared_ptr<HashFinder>             m_hash_finder           = nullptr;


            std::vector<unsigned char> m_pixels;
            unsigned int m_image_width_pixels;
    };
}