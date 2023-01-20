#pragma once

#include<string>
#include<tuple>
#include<vector>
#include<chrono>

namespace PlateSolver   {
    typedef std::tuple<float,float,float,float> AsterismHash;

    typedef std::tuple<std::tuple<float,float,float,float>, unsigned int, unsigned int, unsigned int, unsigned int> AsterismHashWithIndices;

    /**
     * @brief Converts angle from 1.23° form to 1°13'48' string
     *
     * @param x angle in ° degrees
     * @param degree_symbol symbol to be used instead of the decimal dot (normally °, or "h" in case of RA)
     * @return std::string result
     */
    std::string convert_to_deg_min_sec(float x, const std::string &degree_symbol = "°");

    /**
     * @brief Return string with asterism hash in form [Xc,Yc,Xd,Yd]
     */
    std::string hash_tuple_to_string(const AsterismHash &hash);

    float vec_size(float x, float y);

    /**
     * @brief Return vector of asterism hashes (i.e. tuple of 4 floats)
     *
     * @param asterisms_and_hashes_and_indices vector of tuples [asterism_hash, star1_id, star2_id, star3_id, star4_id]
     * @return std::vector<AsterismHash> vector of asterism hashes (i.e. vector of tuple<float,float,float,float>)
     */
    std::vector<AsterismHash> extract_hashes(const std::vector<AsterismHashWithIndices> &asterisms_and_hashes_and_indices);

    template <class X>
    inline X pow2(X x) {return x*x;};

    // if vec2 can be obtained by anti-clock wise rotation of vec1 by (0,pi), the result is positive, otherwise negative
    float get_angle(float vec1_x, float vec1_y, float vec2_x, float vec2_y);


    /**
     * @brief Measure time since the last call of this function and return the string "Bench marking: " + message + "\tTime from previous bench mark: " + time
     *
     * @param message message to show in the result string
     * @return std::string
     */
    std::string bench_mark(const std::string &message);
    static auto last_time = std::chrono::high_resolution_clock::now();

    /**
     * @brief Return file size in bytes
     */
    long long int get_file_size(const std::string &file_address);
}