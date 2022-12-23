#include "../PlateSolver/ImageLoader.h"

#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>

using namespace PlateSolver;
using namespace std;
using namespace cv;


std::vector<unsigned char> PlateSolver::load_bw_image_to_uchar(const std::string &image_address, unsigned int *pixels_per_line)   {
    Mat original_image = imread(image_address);
    Mat bw_image;
    cvtColor(original_image, bw_image, COLOR_BGR2GRAY);
    *pixels_per_line = bw_image.cols;

    vector<unsigned char> result;
    result.reserve(bw_image.rows*bw_image.cols);
    for (int i = 0; i < bw_image.rows; i++) {
        for (int j = 0; j < bw_image.cols; j++) {
            result.push_back(bw_image.at<uchar>(i, j));
        }
    }
    return result;
};


void PlateSolver::dump_to_picture(const std::string &image_address, const std::vector<unsigned char> &pixels, unsigned int pixels_per_line)    {
    int width(pixels_per_line), height(pixels.size()/pixels_per_line);
    cout << width << "x" << height << endl;
    Mat image = Mat::zeros(height, width,0);
    for (unsigned int i_pixel = 0; i_pixel < pixels.size(); i_pixel++)  {
        image.at<uchar>(i_pixel/width,i_pixel % width) = pixels[i_pixel];
    }
    imwrite(image_address, image);
};