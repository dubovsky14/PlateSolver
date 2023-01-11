#include "../PlateSolver/KDTree.h"

#include "../PlateSolver/StringOperations.h"
#include "../PlateSolver/Common.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace PlateSolver;

int main(int argc, const char **argv)   {
    try {
        if (argc != 3)  {
            cout << "Please provide an address of the binary hash file and target .kdtree file";
            return 0;
        }
        const string input_file_address     = argv[1];
        const string outputput_file_address = argv[2];

        ifstream input_file (input_file_address, std::ios::binary | std::ios::out);
        tuple<float,float,float,float> hash;
        unsigned int star_ids[4];
        KDTree kd_tree(get_file_size(input_file_address)/32);
        while(input_file.good())    {

            input_file.read(reinterpret_cast<char *>(&hash), sizeof(hash));
            input_file.read(reinterpret_cast<char *>(&star_ids), sizeof(hash));

            StarIndices stars_id_tuple(star_ids[0],star_ids[1],star_ids[2],star_ids[3]);
            kd_tree.add_point(hash, stars_id_tuple);
        }
        input_file.close();

        kd_tree.create_tree_structure();
        kd_tree.save_to_file(outputput_file_address);

        return 0;
    }
    catch(const string &e)  {
        cout << e << endl;
        abort();
    }
}