#pragma once

#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/StarPositionHandler.h"
#include "../PlateSolver/GeometricTransformations.h"


#include<string>
#include<memory>
#include<vector>

#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>
namespace PlateSolver   {
    class PhotoAnnotator    {
        public:
            PhotoAnnotator( const std::string &star_catalogue_file_numbers,
                            const std::string &star_catalogue_file_names,
                            const std::string &other_catalogues_folder);

            void annotate_photo(const std::string &input_photo_address, const std::string &output_photo_address,
                                unsigned int new_photo_width_pixels, float RA, float dec, float rot, float angular_width,
                                unsigned  int n_stars = 8);

            void annotate_photo(const cv::Mat &input_photo, const std::string &output_photo_address,
                                unsigned int new_photo_width_pixels, float RA, float dec, float rot, float angular_width,
                                unsigned  int n_stars = 8);

            std::shared_ptr<StarDatabaseHandler> get_star_position_handler() const {return m_star_database_handler;};
            std::shared_ptr<StarPositionHandler> get_star_database_handler() const {return m_star_position_handler;};

        private:
            std::shared_ptr<StarDatabaseHandler> m_star_database_handler = nullptr;
            std::shared_ptr<StarPositionHandler> m_star_position_handler = nullptr;

            std::vector<std::unique_ptr<StarDatabaseHandler> > m_deep_sky_objects_databases;
    };
}