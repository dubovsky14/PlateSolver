
#include "../PlateSolver/StarDatabaseHandler.h"
#include "../PlateSolver/StringOperations.h"

#include <fstream>

using namespace std;
using namespace PlateSolver;

StarDatabaseHandler::StarDatabaseHandler(const std::string &csv_file_address)   {
    string line;
    ifstream input_file (csv_file_address);
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
                throw std::string("Unable to read line \"" + line + "\"");
            }

        }
        input_file.close();
    }
    else    {
        throw std::string("Unable to open file \"" + csv_file_address + "\"");
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
