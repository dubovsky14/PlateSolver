#include "../PlateSolver/GraphicsFunctions.h"
#include "../PlateSolver/DescriptionAdder.h"
#include "../PlateSolver/Common.h"

#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>

#include <string>

using namespace std;
using namespace PlateSolver;
using namespace cv;

DescriptionAdder::DescriptionAdder(cv::Mat *image)  {
    m_image = image;
    m_width = m_image->cols;
    m_height = m_image->rows;
    m_font_size = m_width/1920.;
};

DescriptionAdder::DescriptionAdder(cv::Mat *image, float ra_center, float dec_center, float camera_rotation, float angular_width) {
    m_image = image;
    m_width = m_image->cols;
    m_height = m_image->rows;
    m_font_size = m_width/1920.;

    const float angle_per_pixel = angular_width/m_width;

    m_ra_center   = ra_center;
    m_dec_center  = dec_center;
    m_angular_width = angular_width;
    m_angular_height = angular_width*float(m_height)/m_width;

    m_ra_dec_to_pix_convertor = make_shared<RaDecToPixelCoordinatesConvertor>(ra_center, dec_center, -camera_rotation,angle_per_pixel, m_width, m_height);
};

void DescriptionAdder::add_star_description_pixel_coor(int xpos, int ypos, const std::string &label)   {
    const int ypos_text = -ypos - 0.015*m_height;
    unsigned int const text_length_pixels = 0.01*m_width*label.length();
    cout << "offset = " << text_length_pixels << endl;
    const int xpos_text = xpos - text_length_pixels/2;

    putText(*m_image, //target image
            label, //text
            cv::Point(xpos_text, ypos_text), //top-left position
            cv::FONT_HERSHEY_DUPLEX,
            m_font_size,
            CV_RGB(118, 185, 0), //font color
            2);
};

void DescriptionAdder::add_star_description_ra_dec(float ra, float dec, const std::string &label) {
    const tuple<float,float> pix_coor = m_ra_dec_to_pix_convertor->convert_to_pixel_coordinates(
                                                                    Vector3D::get_vector_unity_from_ra_dec(ra,dec),
                                                                    ZeroZeroPoint::upper_left);
    add_star_description_pixel_coor(get<0>(pix_coor), get<0>(pix_coor), label);
};


void DescriptionAdder::add_star_description(const StarDatabaseHandler &star_database_handler, unsigned int n_stars) {
    StarPositionHandler star_position_handler(star_database_handler);
    add_star_description(star_database_handler, star_position_handler, n_stars);
};

void DescriptionAdder::add_star_description(const StarDatabaseHandler &star_database_handler, const StarPositionHandler &star_position_handler, unsigned int n_stars)   {

    // get the stars that we should see in circle around the center of the image
    const auto stars_from_database = star_position_handler.get_stars_around_coordinates(    m_ra_center,
                                                                                            m_dec_center,
                                                                                            0.5*vec_size(m_angular_width,m_angular_height),
                                                                                            true);

    unsigned int plotted_stars = 0;
    for (const tuple<Vector3D, float, unsigned int> &star : stars_from_database) {
        const tuple<float,float> pixel_coor = m_ra_dec_to_pix_convertor->convert_to_pixel_coordinates(get<0>(star), ZeroZeroPoint::upper_left);
        const float pos_x = get<0>(pixel_coor);
        const float pos_y = get<1>(pixel_coor);
        const unsigned int star_id = get<2>(star);
        const std::string star_name = star_database_handler.get_star_name(star_id);

        // skip stars outside of the sensor
        if (pos_x < 0 || pos_x > m_width || pos_y > 0 || -pos_y > m_height) continue;

        cout << pos_x << "\t" << pos_y << "\t" << star_name << endl;

        const float magnitude = get<1>(star);
        add_star_description_pixel_coor(pos_x, pos_y, star_name);
        plotted_stars++;
        if (plotted_stars >= n_stars)   return;
    }
};


void DescriptionAdder::save_image(const std::string &output_address)    {
    imwrite(output_address, *m_image);
};
