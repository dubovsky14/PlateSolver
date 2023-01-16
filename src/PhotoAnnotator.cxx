
#include "../PlateSolver/PhotoAnnotator.h"

#include "../PlateSolver/DescriptionAdder.h"
#include "../PlateSolver/GraphicsFunctions.h"

#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>
#include <filesystem>
#include <iostream>

using namespace std;
using namespace PlateSolver;
using namespace cv;


PhotoAnnotator::PhotoAnnotator( const std::string &star_catalogue_file_numbers,
                                const std::string &star_catalogue_file_names,
                                const std::string &other_catalogues_folder) {

    if (star_catalogue_file_names.length() != 0)    {
        m_star_database_handler = make_shared<StarDatabaseHandler>(star_catalogue_file_numbers, star_catalogue_file_names);
    }
    else {
        m_star_database_handler = make_shared<StarDatabaseHandler>(star_catalogue_file_numbers);
    }

    for (const auto & entry : filesystem::directory_iterator(other_catalogues_folder)) {
        cout << "Adding catalogue: " << entry.path() << endl;
        m_deep_sky_objects_databases.push_back(make_shared<StarDatabaseHandler>(entry.path()));
    }

};

void PhotoAnnotator::annotate_photo(const std::string &input_photo_address, const std::string &output_photo_address,
                                    unsigned int new_photo_width_pixels, float RA, float dec, float rot, float angular_width,
                                    unsigned int n_stars)    {

    Mat original_image = imread(input_photo_address);
    Mat resized_image = get_resized_image(original_image, new_photo_width_pixels);

    DescriptionAdder description_adder(&resized_image, RA, dec, rot, angular_width);

    for (const auto &deep_sky_database : m_deep_sky_objects_databases)  {
        description_adder.add_star_description(*deep_sky_database, 8);
    }

    description_adder.add_star_description(*m_star_database_handler, 8);


    description_adder.save_image(output_photo_address);
};
