#include "../PlateSolver/KDTree.h"

#include "../PlateSolver/StringOperations.h"
#include "../PlateSolver/Common.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace PlateSolver;


int main(int argc, const char **argv)   {
    try {
        if (argc != 3)  {
            cout << "Please provide an address of the csv catalogue file and target binary file";
            return 0;
        }
        const string csv_file_address           = argv[1];
        const string outputput_binary_address   = argv[2];

        string line;
        ifstream input_file (csv_file_address);
        ofstream output_file(outputput_binary_address,std::ios::binary | std::ios::out);
        if (input_file.is_open())    {
            while ( getline (input_file,line) )        {
                StripString(&line);
                if (StartsWith(line, "#") || line.length() == 0)    {
                    continue;
                }
                vector<string> elements = SplitString(line, ",");

                try {
                    const float RA  = std::stod(elements.at(2));
                    const float dec = std::stod(elements.at(3));
                    const float mag = std::stod(elements.at(4));

                    output_file.write(reinterpret_cast<const char *>(&RA),  sizeof(RA));
                    output_file.write(reinterpret_cast<const char *>(&dec), sizeof(dec));
                    output_file.write(reinterpret_cast<const char *>(&mag), sizeof(mag));
                }
                catch(...)  {
                    throw std::string("Unable to read file \"" + csv_file_address + "\"");
                }

            }
            input_file.close();
            output_file.close();
        }
        else    {
            throw std::string("Unable to open file \"" + csv_file_address + "\"");
        }

        return 0;
    }
    catch(const string &e)  {
        cout << e << endl;
        abort();
    }
}