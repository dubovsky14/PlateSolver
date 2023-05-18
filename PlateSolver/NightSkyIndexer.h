#pragma once

#include "../PlateSolver/StarPositionHandler.h"
#include "../PlateSolver/KDTree.h"

#include<string>
#include<map>
#include<tuple>
#include<memory>
#include <fstream>

namespace PlateSolver   {
    class NightSkyIndexer   {
        public:
            NightSkyIndexer(const std::string &star_catalogue);

            NightSkyIndexer(std::shared_ptr<const StarPositionHandler> star_position_handler);

            void create_index_file(const std::string &index_file, float focal_length);

            static void focal_length_to_field_of_view(float focal_length_mm, float *width_radians, float *height_radians);

            // result is vector of tuples, tuple consists of "asterism hash tuple" (tuple of 4 floats), followed by 4 unsigned ints, corresponding to indices of the 4 hashed stars
            void index_sky_region(  float RA, float dec, float angle,
                                    std::map<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>, std::tuple<float,float,float,float> > *result);

        private:
            static std::vector<std::tuple<float, float> > convert_star_coordinates_to_pixels_positions(const std::vector<Vector3D> &stars, float RA, float dec);

            void loop_over_night_sky(float focal_length);

            std::shared_ptr<const StarPositionHandler> m_star_position_handler = nullptr;

            std::unique_ptr<KDTree> m_output_kd_tree            = nullptr;

            void dump_hashes_to_outfile(const std::map<std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>, std::tuple<float,float,float,float> > &result);


    };
}