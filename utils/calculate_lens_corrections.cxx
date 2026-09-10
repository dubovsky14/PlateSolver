#include "../PlateSolver/LensCorrectionCalculator.h"

#include <tuple>
#include <string>
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace PlateSolver;

int main(int argc, const char **argv)   {
    try {
        if (argc != 5)  {
            cout << "Three input arguments are required:\n";
            cout << "\t1st = address of the file with asterisms hashes\n";
            cout << "\t2nd = csv with catalogue of stars\n";
            cout << "\t3rd = address of the photo to plate-solve\n";
            cout << "\t4th = number of lens correction coefficients\n";
            return 0;
        }

        const string hash_file      = argv[1];
        const string star_catalogue = argv[2];
        const string photo_address  = argv[3];
        const int    n_coefficients = std::stoi(argv[4]);

        LensCorrectionCalculator lens_correction_calculator(n_coefficients);
        const LensCorrectionCoefficients lens_correction = lens_correction_calculator.calculate_corrections(photo_address, star_catalogue, hash_file);

        cout << "Corrections calculated:\n";
        cout << "\tc_x = " << lens_correction.c_x << endl;
        cout << "\tc_y = " << lens_correction.c_y << endl;
        cout << "\tk1 = " << lens_correction.k1 << endl;
        cout << "\tk2 = " << lens_correction.k2 << endl;
        cout << "\tk3 = " << lens_correction.k3 << endl;
        cout << "\tsensor_half_diagonal_squared = " << lens_correction.sensor_half_diagonal_squared << endl;

    }
    catch(const runtime_error &e)  {
        cout << e.what() << endl;
        abort();
    }
}