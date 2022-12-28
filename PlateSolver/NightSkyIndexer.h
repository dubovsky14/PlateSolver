#pragma once

#include "../PlateSolver/StarPositionHandler.h"

#include<string>
#include<map>
#include<tuple>
#include<memory>

namespace PlateSolver   {
    class NightSkyIndexer   {
        public:
            NightSkyIndexer(std::shared_ptr<const StarPositionHandler> star_position_handler);

            void create_index_file(const std::string &index_file, float focal_length);

            static void focal_length_to_field_of_view(float focal_length_mm, float *width_radians, float *height_radians);

            // result is vector of tuples, tuple consists of "asterism hash tuple" (tuple of 4 floats), followed by 4 unsigned ints, corresponding to indices of the 4 hashed stars
            void index_sky_region(  float RA, float dec, float angle,
                                    std::vector<std::tuple<std::tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > *result);

            static std::vector<std::tuple<float, float> > convert_star_coordinates_to_pixels_positions(const std::vector<const Vector3D*> &stars, const Vector3D &reference_axis);

        private:
            void loop_over_night_sky();

            std::shared_ptr<const StarPositionHandler> m_star_position_handler = nullptr;


    };
}