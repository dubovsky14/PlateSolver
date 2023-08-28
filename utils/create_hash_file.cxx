#include "../PlateSolver/NightSkyIndexer.h"

#include <tuple>
#include <string>
#include <iostream>
#include <memory>
#include <stdexcept>

using namespace std;
using namespace PlateSolver;

int main(int argc, const char **argv)   {
    try {
        if (argc != 4)  {
            cout << "Three input arguments are required:\n";
            cout << "\t1st = csv with catalogue of stars\n";
            cout << "\t2nd = effective focal length in millimeters\n";
            cout << "\t3rd = output file with asterism hashes\n";
            return 0;
        }

        const string focal_length_string = argv[2];
        try {
            std::stod(focal_length_string);
        }
        catch(...)  {
            throw std::string("Focal length must be floating point number!");
        }

        const string star_catalogue = argv[1];
        const float focal_length    = std::stod(focal_length_string);
        const string hash_file      = argv[3];


        StarDatabaseHandler star_database_handler(star_catalogue);
        shared_ptr<StarPositionHandler> star_position_handler = make_shared<StarPositionHandler>(star_database_handler);

        NightSkyIndexer night_sky_indexer(star_position_handler);
        night_sky_indexer.create_index_file(hash_file, focal_length);

        return 0;
    }
    catch(const runtime_error &e)  {
        cout << e.what() << endl;
        abort();
    }
}