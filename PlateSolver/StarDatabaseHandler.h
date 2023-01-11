#pragma once

#include<string>
#include<vector>

namespace PlateSolver   {
    class StarDatabaseHandler   {
        public:
            StarDatabaseHandler() = delete;

            StarDatabaseHandler(const std::string &star_catalogue_file);

            int get_number_of_stars()   const {return m_vector_RA.size();};

            void get_star_info(unsigned int star_id, float *RA, float *dec, float *mag = nullptr, std::string *name = nullptr)  const;

            std::string get_star_name(unsigned int star_id) const;

            float get_star_ra(unsigned int star_id) const;

            float get_star_dec(unsigned int star_id) const;


        private:
            std::vector<float>          m_vector_RA;
            std::vector<float>          m_vector_dec;
            std::vector<float>          m_vector_mag;
            std::vector<std::string>    m_vector_name;
    };
}