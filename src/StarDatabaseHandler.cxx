
#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/StringOperations.h"
#include "../PlateSolver/Common.h"

#include <fstream>

using namespace std;
using namespace PlateSolver;

StarDatabaseHandler::StarDatabaseHandler(const std::string &star_catalogue_file)   {
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
                    throw std::string("Unable to read file \"" + star_catalogue_file + "\"");
                }

            }
            input_file.close();
        }
        else    {
            throw std::string("Unable to open file \"" + star_catalogue_file + "\"");
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
        throw string("Unkown extension of the star catalogue type. Only .bin or .csv files are allowed");
    }
};


void StarDatabaseHandler::get_star_info(unsigned int star_id, float *RA, float *dec, float *mag, std::string *name) const {
    if (star_id >= m_vector_dec.size()) {
        throw std::string("Requested star ID exceeds the number of stars in the database.");
    }
    *RA     = m_vector_RA[star_id];
    *dec    = m_vector_dec[star_id];

    if (mag)    {
        *mag = m_vector_mag[star_id];
    }
    if (name)    {
        *name = m_vector_name[star_id];
    }
};

std::string StarDatabaseHandler::get_star_name(unsigned int star_id)    const {
    if (star_id >= m_vector_name.size()) {
        throw std::string("Requested star ID exceeds the number of stars in the database.");
    }
    return m_vector_name[star_id];
};

float StarDatabaseHandler::get_star_ra(unsigned int star_id) const  {
    if (star_id >= m_vector_RA.size()) {
        throw std::string("Requested star ID exceeds the number of stars in the database.");
    }
    return m_vector_RA[star_id];
};

float StarDatabaseHandler::get_star_dec(unsigned int star_id)   const    {
    if (star_id >= m_vector_dec.size()) {
        throw std::string("Requested star ID exceeds the number of stars in the database.");
    }
    return m_vector_dec[star_id];
};