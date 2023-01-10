#include "../PlateSolver/HashFinder.h"
#include "../PlateSolver/StringOperations.h"

#include "../PlateSolver/Common.h"
#include "../PlateSolver/KDTree.h"

#include<string>
#include<vector>
#include<tuple>
#include<fstream>
#include<memory>
#include<iostream>

#include<algorithm>


using namespace std;
using namespace PlateSolver;

HashFinder::HashFinder(const string &hash_file) : m_hash_file_address{hash_file}    {

};

vector<vector<tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > >  HashFinder::get_similar_hashes(const vector<tuple<float,float,float,float> > &input_hashes, unsigned int requested_number_of_hashes, std::vector<tuple<unsigned int, unsigned int,float> > *ordering_by_match)  {
    AsterismHashWithIndicesAndDistance result_temp[input_hashes.size()][requested_number_of_hashes];

    AsterismHashWithIndicesAndDistance default_hash{
        tuple<float,float,float,float>({0,0,0,0}),
        0,0,0,0,1e6
    };


    for (unsigned int i_in_hash = 0; i_in_hash < input_hashes.size(); i_in_hash++)  {
        for (unsigned int i_requested_hash = 0; i_requested_hash < requested_number_of_hashes; i_requested_hash++)  {
            result_temp[i_in_hash][i_requested_hash]=default_hash;
        }
    }

    const unsigned int n_last_requested = requested_number_of_hashes-1;

    const bool is_binary_file   = EndsWith(m_hash_file_address, ".bin");
    const bool is_kdtree_file   = EndsWith(m_hash_file_address, ".kdtree");
    const bool is_text_file     = EndsWith(m_hash_file_address, ".txt") || EndsWith(m_hash_file_address, ".csv");

    if (is_kdtree_file) {
        KDTree kd_tree(m_hash_file_address);
        for (unsigned int i_in_hash = 0; i_in_hash < input_hashes.size(); i_in_hash++)  {
            auto hashes_and_indices = kd_tree.get_k_nearest_neighbors(input_hashes[i_in_hash], requested_number_of_hashes);
            for (unsigned int i_requested_hash = 0; i_requested_hash < requested_number_of_hashes; i_requested_hash++)  {
                const float distance2 = calculate_hash_distance_squared(input_hashes[i_in_hash], get<0>(hashes_and_indices[i_in_hash]));
                const auto &star_indices = get<1>(hashes_and_indices[i_requested_hash]);
                result_temp[i_in_hash][i_requested_hash]=AsterismHashWithIndicesAndDistance(
                    get<0>(hashes_and_indices[i_requested_hash]),
                    get<0>(star_indices), get<1>(star_indices), get<2>(star_indices), get<3>(star_indices),
                    distance2
                );
            }
        }

    }


    if (is_binary_file) {
        unsigned int star_ids[4];
        tuple<float,float,float,float> hash;

        ifstream input_file (m_hash_file_address, std::ios::binary | std::ios::out);
        while(input_file.good())    {

            input_file.read(reinterpret_cast<char *>(&hash), sizeof(hash));
            input_file.read(reinterpret_cast<char *>(star_ids), sizeof(hash));

            for (unsigned int i_in_hash = 0; i_in_hash < input_hashes.size(); i_in_hash++)  {
                const float distance2 = calculate_hash_distance_squared(hash, input_hashes[i_in_hash]);
                if (distance2 < get<5>(result_temp[i_in_hash][n_last_requested]))    {
                    result_temp[i_in_hash][n_last_requested] = tuple<tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int, float>({
                        hash, star_ids[0], star_ids[1], star_ids[2], star_ids[3], distance2
                    });
                    sort(&result_temp[i_in_hash][0], &result_temp[i_in_hash][requested_number_of_hashes], [](const auto &a, const auto &b) {
                        return get<5>(b) > get<5>(a);
                    });
                }
            }
        }
        input_file.close();
    }

    if (is_text_file)    {
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

    if (ordering_by_match != nullptr)   {
        typedef tuple<unsigned int, unsigned int,float> CoordinatesAndDistance;
        ordering_by_match->reserve(input_hashes.size()*requested_number_of_hashes);
        for (unsigned int i_in_hash = 0; i_in_hash < input_hashes.size(); i_in_hash++)  {
            for (unsigned int i_requested_hash = 0; i_requested_hash < requested_number_of_hashes; i_requested_hash++)  {
                ordering_by_match->push_back(CoordinatesAndDistance(i_in_hash,
                                                                    i_requested_hash,
                                                                    get<5>(result_temp[i_in_hash][i_requested_hash])));
            }
        }

        sort(ordering_by_match->begin(), ordering_by_match->end(), [](const CoordinatesAndDistance &a, const CoordinatesAndDistance &b) {
            const float dist_a = get<2>(a);
            const float dist_b = get<2>(b);
            return dist_b > dist_a;
        });
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
