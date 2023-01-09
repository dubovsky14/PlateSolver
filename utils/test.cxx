#include "../PlateSolver/KDTree.h"

#include "../PlateSolver/StringOperations.h"
#include "../PlateSolver/Common.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace PlateSolver;


long long int get_file_size(const std::string &file_address) {
    ifstream file(file_address, ios::binary);

    // if file does not exist
    if (file.fail())    return -1;

    const auto begin = file.tellg();
    file.seekg (0, ios::end);
    const auto end = file.tellg();
    file.close();
    return (end-begin);
};


int main(int argc, const char **argv)   {
    try {
        if (argc != 2)  {
            cout << "Please provide an address of the hash file";
            return 0;
        }

        const string binary_file   = argv[1];

        ifstream input_file (binary_file, std::ios::binary | std::ios::out);
        tuple<float,float,float,float> hash;
        tuple<unsigned int, unsigned int> stars_AB;
        tuple<unsigned int, unsigned int> stars_CD;

        KDTree kd_tree(get_file_size(binary_file)/32);
        while(input_file.good())    {

            input_file.read(reinterpret_cast<char *>(&hash), sizeof(hash));
            input_file.read(reinterpret_cast<char *>(&stars_AB), sizeof(stars_AB));
            input_file.read(reinterpret_cast<char *>(&stars_CD), sizeof(stars_CD));

            kd_tree.add_point(hash, stars_AB);
        }
        input_file.close();

        cout << "Hashes loaded, going to create kd-tree\n";
        kd_tree.create_tree_structure();
        cout << "kd-tree created. Please provide type the hash:\n";

        while(true) {
            cout << "Please provide type the hash:\n";
            string hash_string;
            cin >> hash_string;
            vector<string> elements = SplitAndStripString(hash_string, ",");
            if (elements.size() != 4)   {
                cout << "Invalid hash\n";
                continue;
            }
            tuple<float,float,float,float> hash(std::stod(elements[0]), std::stod(elements[1]),std::stod(elements[2]),std::stod(elements[3]));

            cout << "input hash: " << hash_tuple_to_string(hash) << endl;
            const auto result = kd_tree.get_k_nearest_neighbor(hash);
            cout << "[ " << get<0>(get<0>(result)) << ","  << get<1>(get<0>(result)) << ","  << get<2>(get<0>(result)) << ","  << get<3>(get<0>(result)) << " ],"
                    << get<0>(get<1>(result)) << "," << get<1>(get<1>(result)) << endl;
        }


        return 0;
    }
    catch(const string &e)  {
        cout << e << endl;
        abort();
    }
}