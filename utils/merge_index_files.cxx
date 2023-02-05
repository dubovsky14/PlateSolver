#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <filesystem>
#include <map>

#include "../PlateSolver/KDTree.h"

using namespace std;
using namespace PlateSolver;

void merge_index_files(const vector<string> &input_files, const string output_file) {
    unsigned long long input_files_total_size = 0;
    for (const string &input_file : input_files) {
        input_files_total_size += filesystem::file_size(input_file);
    }
    KDTree merged_tree (input_files_total_size/sizeof(PointInKDTree));
    map<StarIndices, char> combinations_already_added;

    for (const string &input_file : input_files) {
        KDTree input_tree(input_file);
        cout << "Merging file: " << input_file << endl;
        const unsigned long long points_in_input_tree = input_tree.get_number_of_points_in_tree();
        float           coordinates_array[4];
        StarIndices     star_indices;

        for (unsigned long long i = 0; i < points_in_input_tree; i++) {
            input_tree.get_point(i, coordinates_array, &star_indices);
            tuple<float, float, float, float> coordinates = make_tuple(coordinates_array[0], coordinates_array[1], coordinates_array[2], coordinates_array[3]);
            if (combinations_already_added.find(star_indices) != combinations_already_added.end()) {
                continue;
            }
            merged_tree.add_point(coordinates, star_indices);
            combinations_already_added[star_indices] = 1;
        }
    }
    merged_tree.create_tree_structure();
    merged_tree.save_to_file(output_file);
}


int main(int argc, const char **argv)   {
    try {
        if (argc < 3)  {
            cout << "At least two input arguments are required:\n";
            cout << "\t1st = address of the output merged file\n";
            cout << "\t2nd - Nth = addresses of the input files to be merged\n";
            return 0;
        }
        vector<string> input_files;
        for (int i = 2; i < argc; i++) {
            input_files.push_back(argv[i]);
        }
        const string output_file = argv[1];

        cout << "Output file: " << output_file << endl;
        cout << "Input files:\n";
        for (const string &input_file : input_files) {
            cout << input_file << endl;
        }

        const bool output_file_exists = filesystem::exists(output_file);
        if (output_file_exists) {
            throw std::string("Output file already exists!");
        }

        for (const string &input_file : input_files) {
            const bool input_file_exists = filesystem::exists(input_file);
            if (!input_file_exists) {
                throw std::string("Input file does not exist!");
            }
        }

        cout << "Merging files...\n";
        merge_index_files(input_files, output_file);

    }
    catch(const string &e)  {
        cout << e << endl;
        abort();
    }
}