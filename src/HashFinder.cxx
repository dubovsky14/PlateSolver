#include "../PlateSolver/HashFinder.h"
#include "../PlateSolver/StringOperations.h"

#include "../PlateSolver/Common.h"

#include<string>
#include<vector>
#include<tuple>
#include<fstream>
#include<memory>

#include<algorithm>


using namespace std;
using namespace PlateSolver;

HashFinder::HashFinder(const string &hash_file) : m_hash_file_address{hash_file}    {

};

vector<vector<tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > >  HashFinder::get_similar_hashes(const vector<tuple<float,float,float,float> > &input_hashes, unsigned int requested_number_of_hashes)  {
    tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int, float> result_temp[input_hashes.size()][requested_number_of_hashes];

    tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int, float> default_hash{
        tuple<float,float,float,float>({0,0,0,0}),
        0,0,0,0,1e6
    };


    for (unsigned int i_in_hash = 0; i_in_hash < input_hashes.size(); i_in_hash++)  {
        for (unsigned int i_requested_hash = 0; i_requested_hash < requested_number_of_hashes; i_requested_hash++)  {
            result_temp[i_in_hash][i_requested_hash]=default_hash;
        }
    }

    const unsigned int n_last_requested = requested_number_of_hashes-1;

    const bool is_binary_file = EndsWith(m_hash_file_address, ".bin");

    if (is_binary_file) {
        unsigned int starA,starB,starC,starD;
        tuple<float,float,float,float> hash;

        ifstream input_file (m_hash_file_address, std::ios::binary | std::ios::out);
        while(input_file.good())    {
            input_file  >>  std::noskipws >> get<0>(hash) >> get<1>(hash)  >> get<2>(hash)  >> get<3>(hash);
            input_file  >>  std::noskipws >> starA >> starB >> starC >> starD;

            for (unsigned int i_in_hash = 0; i_in_hash < input_hashes.size(); i_in_hash++)  {
                const float distance2 = calculate_hash_distance_squared(hash, input_hashes[i_in_hash]);
                if (distance2 < get<5>(result_temp[i_in_hash][n_last_requested]))    {
                    result_temp[i_in_hash][n_last_requested] = tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int, float>({
                        hash, starA, starB, starC, starD, distance2
                    });
                    sort(&result_temp[i_in_hash][0], &result_temp[i_in_hash][requested_number_of_hashes], [](const auto &a, const auto &b) {
                        return get<5>(b) > get<5>(a);
                    });
                }
            }
        }
        input_file.close();
    }





    if (!is_binary_file)    {
        string line;
        ifstream input_file (m_hash_file_address);
        if (input_file.is_open())    {
            while ( getline (input_file,line) )        {
                StripString(&line);
                if (StartsWith(line, "#") || line.length() == 0)    {
                    continue;
                }
                vector<string> elements = SplitString(line, ",");
                if (elements.size() != 8)   {
                    continue;
                }

                tuple<float,float,float,float> hash ({
                    std::stod(elements[0]),
                    std::stod(elements[1]),
                    std::stod(elements[2]),
                    std::stod(elements[3]),
                });

                const unsigned int starA = std::stoi(elements[4]);
                const unsigned int starB = std::stoi(elements[5]);
                const unsigned int starC = std::stoi(elements[6]);
                const unsigned int starD = std::stoi(elements[7]);

                for (unsigned int i_in_hash = 0; i_in_hash < input_hashes.size(); i_in_hash++)  {
                    const float distance2 = calculate_hash_distance_squared(hash, input_hashes[i_in_hash]);
                    if (distance2 < get<5>(result_temp[i_in_hash][n_last_requested]))    {
                        result_temp[i_in_hash][n_last_requested] = tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int, float>({
                            hash, starA, starB, starC, starD, distance2
                        });
                        sort(&result_temp[i_in_hash][0], &result_temp[i_in_hash][requested_number_of_hashes], [](const auto &a, const auto &b) {
                            return get<5>(b) > get<5>(a);
                        });
                    }
                }
            }
            input_file.close();
        }
        else    {
            throw std::string("Unable to open file \"" + m_hash_file_address + "\"");
        }
    }

    vector<vector<tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > > result(input_hashes.size());
    for (unsigned int i_in_hash = 0; i_in_hash < input_hashes.size(); i_in_hash++)  {
        for (unsigned int i_requested_hash = 0; i_requested_hash < requested_number_of_hashes; i_requested_hash++)  {
            const auto &x = result_temp[i_in_hash][i_requested_hash];
                result[i_in_hash].push_back(
                tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int>({
                    get<0>(x),
                    get<1>(x),
                    get<2>(x),
                    get<3>(x),
                    get<4>(x),
                })
            );
        }
    }


    return result;
};


float HashFinder::calculate_hash_distance_squared(const std::tuple<float,float,float,float> &hash1, const std::tuple<float,float,float,float> &hash2)   {
    return  pow2(get<0>(hash1) - get<0>(hash2)) +
            pow2(get<1>(hash1) - get<1>(hash2)) +
            pow2(get<2>(hash1) - get<2>(hash2)) +
            pow2(get<3>(hash1) - get<3>(hash2));
};

