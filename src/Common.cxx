#include "../PlateSolver/Common.h"

#include <cmath>
#include <iostream>
#include <fstream>

using namespace PlateSolver;
using namespace std;

std::string PlateSolver::convert_to_deg_min_sec(float x, const std::string &degree_symbol)    {
    string result = "";
    if (x <0) {
        result = "-";
        x *= -1;
    }
    result = result + to_string(int(x)) + degree_symbol;
    x -= int(x);
    result = result + to_string(int(60*x)) + "\"";
    x = 60*(x*60-int(x*60));
    result = result + to_string(x) + "\'";
    return result;
};

std::string PlateSolver::hash_tuple_to_string(const AsterismHash &hash) {
    string result = "[";
    result = result + to_string(get<0>(hash)) + ", ";
    result = result + to_string(get<1>(hash)) + ", ";
    result = result + to_string(get<2>(hash)) + ", ";
    result = result + to_string(get<3>(hash)) + "]";
    return result;
};

float PlateSolver::vec_size(float x, float y)   {
    return sqrt(pow2(x) + pow2(y));
};

std::vector<AsterismHash> PlateSolver::extract_hashes(const std::vector<AsterismHashWithIndices> &asterisms_and_hashes_and_indices)  {
    std::vector<AsterismHash> result(asterisms_and_hashes_and_indices.size());
    for (unsigned int i_hash = 0; i_hash < asterisms_and_hashes_and_indices.size(); i_hash++)   {
        result[i_hash] = get<0>(asterisms_and_hashes_and_indices[i_hash]);
    }
    return result;
};

float PlateSolver::get_angle(float vec1_x, float vec1_y, float vec2_x, float vec2_y)    {
    const float vector_product_z = (vec1_x*vec2_y - vec2_x*vec1_y);
    const float scalar_product = vec1_x*vec2_x + vec1_y*vec2_y;
    const float size1 = sqrt(pow2(vec1_x) + pow2(vec1_y));
    const float size2 = sqrt(pow2(vec2_x) + pow2(vec2_y));
    if (scalar_product > 0) {
        return asin(vector_product_z/(size1*size2));
    }
    else {
        return vector_product_z > 0 ? acos(scalar_product/(size1*size2)) : -acos(scalar_product/(size1*size2));
    }
};

string PlateSolver::bench_mark(const std::string &message) {

    auto time_now = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(time_now - last_time);
    last_time = time_now;
    return "Bench marking: " + message + "\tTime from previous bench mark: " + std::to_string(duration.count()/1000.);
}

long long int PlateSolver::get_file_size(const std::string &file_address) {
    ifstream file(file_address, ios::binary);

    // if file does not exist
    if (file.fail())    return -1;

    const auto begin = file.tellg();
    file.seekg (0, ios::end);
    const auto end = file.tellg();
    file.close();
    return (end-begin);
};
