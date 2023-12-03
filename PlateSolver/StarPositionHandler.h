#pragma once

#include "../PlateSolver/Vector3D.h"
#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/KDTree_templated.h"

#include<tuple>
#include<vector>
#include<memory>


namespace PlateSolver   {
    class StarPositionHandler   {
        public:
            StarPositionHandler(unsigned int approximate_number_of_stars);

            StarPositionHandler(const StarDatabaseHandler &star_database_handler);

            void AddStar(float RA, float dec, float mag, unsigned int id);

            void BuildTree();

            // returns vector of tuples, representing stars around provided point. Tuple content: (star 3D vector, mag, ID)
            std::vector<std::tuple<Vector3D, float, unsigned int> >  get_stars_around_coordinates(float RA, float dec, float angle, bool sort_by_magnitude = true)   const;


        private:
            // query point are x,y,z coordinates on unite sphere, additional info is tuple of this magnitude and ID
            std::unique_ptr<KDTree_t<float, std::tuple<float, unsigned int>>>  m_kd_tree = nullptr;


    };
}