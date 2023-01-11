#pragma once

#include "../PlateSolver/StarPositionHandler.h"
#include "../PlateSolver/KDTree.h"

#include<string>
#include<map>
#include<tuple>
#include<memory>
#include <fstream>

namespace PlateSolver   {
    enum class OutputType{text_file, binary_file, kd_tree_file};
    class NightSkyIndexer   {
        public:
            NightSkyIndexer(const std::string &star_catalogue);

            NightSkyIndexer(std::shared_ptr<const StarPositionHandler> star_position_handler);

            void create_index_file(const std::string &index_file, float focal_length);

            static void focal_length_to_field_of_view(float focal_length_mm, float *width_radians, float *height_radians);

            // result is vector of tuples, tuple consists of "asterism hash tuple" (tuple of 4 floats), followed by 4 unsigned ints, corresponding to indices of the 4 hashed stars
            void index_sky_region(  float RA, float dec, float angle,
                                    std::vector<std::tuple<std::tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > *result);

        private:
            static std::vector<std::tuple<float, float> > convert_star_coordinates_to_pixels_positions(const std::vector<Vector3D> &stars, float RA, float dec);

            void loop_over_night_sky(float focal_length);

            std::shared_ptr<const StarPositionHandler> m_star_position_handler = nullptr;

            std::shared_ptr<std::ofstream> m_output_hash_file   = nullptr;
            std::shared_ptr<KDTree> m_output_kd_tree            = nullptr;

            OutputType m_output_type = OutputType::kd_tree_file;

            void dump_hash_vector_to_outfile(const std::vector<std::tuple<std::tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > &hash_vector);


    };
}