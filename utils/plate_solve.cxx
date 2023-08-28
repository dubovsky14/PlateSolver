#include "../PlateSolver/PlateSolverTool.h"

#include <tuple>
#include <string>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace PlateSolver;

int main(int argc, const char **argv)   {
    try {
        if (argc != 4)  {
            cout << "Three input arguments are required:\n";
            cout << "\t1st = address of the file with asterisms hashes\n";
            cout << "\t2nd = csv with catalogue of stars\n";
            cout << "\t3rd = address of the photo to plate-solve\n";
            return 0;
        }

        const string hash_file      = argv[1];
        const string star_catalogue = argv[2];
        const string photo_address  = argv[3];

        PlateSolverTool plate_solver_tool(hash_file, star_catalogue);

        const tuple<float,float,float,float,float> result = plate_solver_tool.plate_solve(photo_address);
        const float RA      = get<0>(result);
        const float dec     = get<1>(result);
        const float rot     = (180/M_PI)*get<2>(result);
        const float width   = (180/M_PI)*get<3>(result);
        const float height  = (180/M_PI)*get<4>(result);

        if (width == 0) {
            cout << "Plate solving failed!\n";
            return 0;
        }

        cout << "Plate solving finished\n";
        cout << "\tRA = " << convert_to_deg_min_sec(RA, "h") << endl;
        cout << "\tdec = " << convert_to_deg_min_sec(dec) << endl;
        cout << "\trot = " << convert_to_deg_min_sec(rot) << endl;
        cout << "\twidth x height = " << convert_to_deg_min_sec(width) + " x " << convert_to_deg_min_sec(height) << endl;

        return 0;
    }
    catch(const runtime_error &e)  {
        cout << e.what() << endl;
        abort();
    }
}