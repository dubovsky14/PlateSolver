#include "../PlateSolver/GraphicsFunctions.h"


#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>

using namespace std;
using namespace PlateSolver;
using namespace cv;

Mat PlateSolver::get_resized_image(const Mat &original_image, unsigned int maximal_width)    {
    const unsigned int width_original = original_image.cols;
    const unsigned int height_original = original_image.rows;

    const unsigned int width_resized = min(width_original, maximal_width);
    const float scale = float(width_resized)/float(width_original);

    Mat resized_image;
    resize(original_image, resized_image, Size(scale*width_original, scale*height_original), INTER_LINEAR);
    return resized_image;
}

