
#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/StringOperations.h"
#include "../PlateSolver/Common.h"

#include <fstream>
#include <stdexcept>

using namespace std;
using namespace PlateSolver;

StarDatabaseHandler::StarDatabaseHandler(const std::string &star_catalogue_file)   {
    load_csv_or_number_bin_file(star_catalogue_file);
}

StarDatabaseHandler::StarDatabaseHandler(const std::string &star_catalogue_bin_file_numbers, const std::string &star_catalogue_bin_file_names)   {
    if (!EndsWith(star_catalogue_bin_file_numbers, ".bin")) {
        throw runtime_error("StarDatabaseHandler::the first argument in 2-arguments constructor is supposed to be .bin file!");
    }
    load_csv_or_number_bin_file(star_catalogue_bin_file_numbers);
    m_catalogue_file_names = make_unique<ifstream>(star_catalogue_bin_file_names, std::ios::binary | std::ios::out);
    m_catalogue_file_names->read(reinterpret_cast<char *>(&m_max_length_name), sizeof(m_max_length_name));
    m_number_of_star_names = m_max_length_name != 0 ?
                            (get_file_size(star_catalogue_bin_file_names)-sizeof(m_max_length_name))/m_max_length_name : 0;
}


void StarDatabaseHandler::load_csv_or_number_bin_file(const std::string &star_catalogue_file)  {
    if (EndsWith(star_catalogue_file, ".csv")) {
        string line;
        ifstream input_file (star_catalogue_file);
        if (input_file.is_open())    {
            while ( getline (input_file,line) )        {
                StripString(&line);
                if (StartsWith(line, "#") || line.length() == 0)    {
                    continue;
                }
                vector<string> elements = SplitString(line, ",");

                try {
                    m_vector_name.push_back( elements.at(1)             );
                    m_vector_RA  .push_back( std::stod(elements.at(2))  );
                    m_vector_dec .push_back( std::stod(elements.at(3))  );
                    m_vector_mag .push_back( std::stod(elements.at(4))  );
                }
                catch(...)  {
                    throw runtime_error("Unable to read file \"" + star_catalogue_file + "\"");
                }

            }
            input_file.close();
        }
        else    {
            throw runtime_error("Unable to open file \"" + star_catalogue_file + "\"");
        }
    }
    else if (EndsWith(star_catalogue_file, ".bin")) {

        ifstream input_file (star_catalogue_file, std::ios::binary | std::ios::out);
        float RA_dec_mag[3];
        const unsigned int n_stars = get_file_size(star_catalogue_file)/12;
        m_vector_RA  .reserve(n_stars);
        m_vector_dec .reserve(n_stars);
        m_vector_mag .reserve(n_stars);

        while(input_file.good())    {
            input_file.read(reinterpret_cast<char *>(&RA_dec_mag), sizeof(RA_dec_mag));
            m_vector_RA  .push_back( RA_dec_mag[0] );
            m_vector_dec .push_back( RA_dec_mag[1] );
            m_vector_mag .push_back( RA_dec_mag[2] );

        }
        input_file.close();
    }
    else {
        throw runtime_error("Unkown extension of the star catalogue type. Only .bin or .csv files are allowed");
    }
};


void StarDatabaseHandler::get_star_info(unsigned int star_id, float *RA, float *dec, float *mag, std::string *name) const {
    if (star_id >= m_vector_dec.size()) {
        throw runtime_error("Requested star ID exceeds the number of stars in the database.");
    }
    *RA     = m_vector_RA[star_id];
    *dec    = m_vector_dec[star_id];

    if (mag)    {
        *mag = m_vector_mag[star_id];
    }
    if (name)    {
        *name = get_star_name(star_id);
    }
};

std::string StarDatabaseHandler::get_star_name(unsigned int star_id)    const {
    if (m_catalogue_file_names == nullptr)  {
        if (star_id >= m_vector_name.size()) {
            return "";
        }
        return m_vector_name[star_id];
    }
    else {
        if (star_id >= m_number_of_star_names)  {
            return "";
        }
        char name_char[m_max_length_name];
        m_catalogue_file_names->seekg(sizeof(m_max_length_name) + star_id*m_max_length_name);
        m_catalogue_file_names->read(reinterpret_cast<char *>(&name_char), sizeof(name_char));
        string result;
        for (unsigned int i_char = 0; i_char < m_max_length_name; i_char++) result = result + name_char[i_char];
        StripString(&result);
        return result;

    }
};

float StarDatabaseHandler::get_star_ra(unsigned int star_id) const  {
    if (star_id >= m_vector_RA.size()) {
        throw runtime_error("Requested star ID exceeds the number of stars in the database.");
    }
    return m_vector_RA[star_id];
};

float StarDatabaseHandler::get_star_dec(unsigned int star_id)   const    {
    if (star_id >= m_vector_dec.size()) {
        throw runtime_error("Requested star ID exceeds the number of stars in the database.");
    }
    return m_vector_dec[star_id];
};