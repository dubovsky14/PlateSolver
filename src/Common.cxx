#include "../PlateSolver/Common.h"

using namespace PlateSolver;
using namespace std;

std::string PlateSolver::convert_to_ged_min_sec(float x)    {
    string result = to_string(int(x)) + "°";
    x -= int(x);
    result = result + to_string(int(60*x)) + "\"";
    x = 60*(x*60-int(x*60));
    result = result + to_string(x) + "\'";
    return result;
};

std::string PlateSolver::hash_tuple_to_string(std::tuple<float,float,float,float> hash) {
    string result = "[";
    result = result + to_string(get<0>(hash)) + ", ";
    result = result + to_string(get<1>(hash)) + ", ";
    result = result + to_string(get<2>(hash)) + ", ";
    result = result + to_string(get<3>(hash)) + "]";
    return result;
};