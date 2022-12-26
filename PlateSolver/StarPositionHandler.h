#pragma once

#include "../PlateSolver/Vector3D.h"
#include "../PlateSolver/StarDatabaseHandler.h"

#include<tuple>
#include<vector>


namespace PlateSolver   {
    class StarPositionHandler   {
        public:
            StarPositionHandler(unsigned int approximate_number_of_stars);

            StarPositionHandler(const StarDatabaseHandler &star_database_handler);

            void AddStar(float RA, float dec, float mag, unsigned int id);

            // returns vector of tuples, representing stars around provided point. Tuple content: (star 3D vector, mag, ID)
            std::vector<std::tuple<Vector3D, float, unsigned int> >  get_stars_around_coordinates(float RA, float dec, float angle, bool sort_by_magnitude = true)   const;


        private:
            void reserve(unsigned int approximate_number_of_stars);

            std::vector<Vector3D>       m_star_positions;
            std::vector<unsigned int>   m_star_IDs;
            std::vector<float>          m_star_magnitudes;


    };
}