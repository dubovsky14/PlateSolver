#pragma once

#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/HashFinder.h"
#include "../PlateSolver/Common.h"
#include "../PlateSolver/NightSkyIndexer.h"
#include "../PlateSolver/StarPositionHandler.h"


#include<vector>
#include<string>
#include<tuple>
#include<memory>

namespace   PlateSolver {
    class PlateSolverTool   {
        public:
            PlateSolverTool() = delete;

            PlateSolverTool(const std::string &hash_file, const std::string &stars_catalogue);

            // returns : [RA, dec, rotation (zero means upwards is towards the north celestial pole), width in radians, height in radians]
            std::tuple<float,float,float,float,float> plate_solve(const std::string &jpg_file);

            // returns : [RA, dec, rotation (zero means upwards is towards the north celestial pole), width in radians, height in radians]
            std::tuple<float,float,float,float,float> get_hypothesis_coordinates(   float pos_x_starA, float pos_y_starA, unsigned int id_starA,
                                                                                    float pos_x_starB, float pos_y_starB, unsigned int id_starB,
                                                                                    float image_width, float image_height);

            std::vector<AsterismHashWithIndices> get_hashes_with_indices(const std::vector<std::tuple<float,float,float> > &stars, unsigned nstars);

            static std::vector<std::tuple<float,float,float> > select_stars_around_point(const std::vector<std::tuple<float,float,float> > &stars_all,
                                                                                        float point_x, float point_y, float radius);

            bool validate_hypothesis(   const std::vector<std::tuple<float,float,float> > &stars_around_center,
                                        const std::tuple<float,float,float,float,float> &hypothesis_coordinates);
        private:

            std::shared_ptr<StarDatabaseHandler>    m_star_database_handler = nullptr;
            std::shared_ptr<HashFinder>             m_hash_finder           = nullptr;
            std::shared_ptr<StarPositionHandler>    m_star_position_handler = nullptr;
            std::shared_ptr<NightSkyIndexer>        m_night_sky_indexer     = nullptr;


            std::vector<unsigned char> m_pixels;
            unsigned int m_image_width_pixels;
            unsigned int m_image_height_pixels;
    };
}