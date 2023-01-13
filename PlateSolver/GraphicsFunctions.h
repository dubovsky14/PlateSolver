#pragma once

#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>


namespace PlateSolver   {
    cv::Mat get_resized_image(const cv::Mat &original_image, unsigned int maximal_width);
}