#pragma once

#include<string>
#include<vector>
#include<tuple>

namespace PlateSolver   {
    class HashFinder    {
        public:
            HashFinder(const std::string &hash_file);

            std::vector<std::vector<std::tuple<std::tuple<float,float,float,float>,unsigned int, unsigned int, unsigned int, unsigned int> > >  get_similar_hashes(const std::vector<std::tuple<float,float,float,float> > &input_hashes, unsigned int requested_number_of_hashes);

            static float calculate_hash_distance_squared(const std::tuple<float,float,float,float> &hash1, const std::tuple<float,float,float,float> &hash2);
        private:
            std::string m_hash_file_address;

            static float pow2(float x)  {return x*x;};

    };
}