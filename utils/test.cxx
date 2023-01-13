#include "../PlateSolver/PlateSolverTool.h"
#include "../PlateSolver/Common.h"
#include "../PlateSolver/GraphicsFunctions.h"
#include "../PlateSolver/DescriptionAdder.h"

#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>


#include <tuple>
#include <string>
#include <iostream>

using namespace std;
using namespace PlateSolver;
using namespace cv;

int main(int argc, const char **argv)   {
    try {
        bench_mark("start");
        if (argc != 3)  {
            cout << "Two input arguments are required:\n";
            cout << "\t1st = input image\n";
            cout << "\t2nd = output image\n";
            return 0;
        }
        const string input_image_address  = argv[1];
        const string output_image_address = argv[2];

        const string hash_file = "../data/index_file_840mm.kdtree";
        const string star_catalogue_file = "../data/catalogue.bin";

        PlateSolverTool plate_solver_tool(hash_file, star_catalogue_file);

        const tuple<float,float,float,float,float> result = plate_solver_tool.plate_solve(input_image_address);
        const float RA      = get<0>(result);
        const float dec     = get<1>(result);
        const float rot     = get<2>(result);
        const float width   = get<3>(result);
        const float height  = get<4>(result);

        cout << "Plate solving finished\n";
        cout << "\tRA = " << convert_to_deg_min_sec(RA, "h") << endl;
        cout << "\tdec = " << convert_to_deg_min_sec(dec) << endl;
        cout << "\trot = " << convert_to_deg_min_sec(rot) << endl;
        cout << "\twidth x height = " << convert_to_deg_min_sec(width) + " x " << convert_to_deg_min_sec(height) << endl;


        bench_mark("plate-solving finished");
        Mat original_image = imread(input_image_address);
        const unsigned int width_pixels = 1400;
        Mat resized_image = get_resized_image(original_image, width_pixels);

        bench_mark("image loaded and rescaled");
        StarDatabaseHandler star_database_handler("../data/catalogue.csv");
        bench_mark("catalogue loaded");
        DescriptionAdder    description_adder(&resized_image, RA, dec, rot, width);
        const float c = width_pixels/1920.;
        description_adder.add_star_description(star_database_handler, 8);
        description_adder.save_image(output_image_address);
        bench_mark("image saved");

        return 0;
    }
    catch(const string &e)  {
        cout << e << endl;
        abort();
    }
}