#pragma once

#include<string>
#include<vector>
#include<memory>
#include<fstream>

namespace PlateSolver   {
    class StarDatabaseHandler   {
        public:
            StarDatabaseHandler() = delete;

            StarDatabaseHandler(const std::string &star_catalogue_file);

            StarDatabaseHandler(const std::string &star_catalogue_bin_file_numbers, const std::string &star_catalogue_bin_file_names);

            int get_number_of_stars()   const {return m_vector_RA.size();};

            void get_star_info(unsigned int star_id, float *RA, float *dec, float *mag = nullptr, std::string *name = nullptr)  const;

            std::string get_star_name(unsigned int star_id) const;

            float get_star_ra(unsigned int star_id) const;

            float get_star_dec(unsigned int star_id) const;


        private:
            void load_csv_or_number_bin_file(const std::string &star_catalogue_file);

            std::vector<float>          m_vector_RA;
            std::vector<float>          m_vector_dec;
            std::vector<float>          m_vector_mag;
            std::vector<std::string>    m_vector_name;


            std::shared_ptr<std::ifstream> m_catalogue_file_names = nullptr;
            unsigned int m_max_length_name = 0;
            unsigned int m_number_of_star_names = 0;
    };
}