#pragma once

#include "../PlateSolver/Common.h"

#include<string>
#include<vector>
#include<tuple>

namespace PlateSolver   {
    class HashFinder    {
        public:
            HashFinder()    = delete;

            explicit HashFinder(const std::string &hash_file);

            HashFinder(const HashFinder &a) = default;

            std::vector<std::vector<std::tuple<std::tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > >  get_similar_hashes(const std::vector<std::tuple<float,float,float,float> > &input_hashes, unsigned int requested_number_of_hashes);

            inline static float calculate_hash_distance_squared(const std::tuple<float,float,float,float> &hash1, const std::tuple<float,float,float,float> &hash2) {
                return  pow2(std::get<0>(hash1) - std::get<0>(hash2)) +
                        pow2(std::get<1>(hash1) - std::get<1>(hash2)) +
                        pow2(std::get<2>(hash1) - std::get<2>(hash2)) +
                        pow2(std::get<3>(hash1) - std::get<3>(hash2));
            };


        private:
            std::string m_hash_file_address;

    };
}