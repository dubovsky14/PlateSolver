#pragma once

#include "../PlateSolver/Common.h"

#include<string>
#include<vector>
#include<tuple>

namespace PlateSolver   {
    typedef std::tuple<AsterismHash, unsigned int, unsigned int, unsigned int, unsigned int, float> AsterismHashWithIndicesAndDistance;
    class HashFinder    {
        public:
            HashFinder()    = delete;

            explicit HashFinder(const std::string &hash_file);

            HashFinder(const HashFinder &a) = default;

            // look at input hashes and for each of them, return vector of hashes with corresponding star ids from the pre-calculated file
            // ir ordering ordering_by_match != nullptr, it will push into this pairs indices, where 0-th element are the indices for the best match, 1-st element is 2nd match etc.
            std::vector<std::vector<AsterismHashWithIndices>>  get_similar_hashes(  const std::vector<AsterismHash> &input_hashes,
                                                                                    unsigned int requested_number_of_hashes,
                                                                                    std::vector<std::tuple<unsigned int, unsigned int,float> > *ordering_by_match = nullptr);

            inline static float calculate_hash_distance_squared(const AsterismHash &hash1, const AsterismHash &hash2) {
                return  pow2(std::get<0>(hash1) - std::get<0>(hash2)) +
                        pow2(std::get<1>(hash1) - std::get<1>(hash2)) +
                        pow2(std::get<2>(hash1) - std::get<2>(hash2)) +
                        pow2(std::get<3>(hash1) - std::get<3>(hash2));
            };


        private:
            std::string m_hash_file_address;

    };
}