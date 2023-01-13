#include "../PlateSolver/KDTree.h"

#include "../PlateSolver/StringOperations.h"
#include "../PlateSolver/Common.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <string>


using namespace std;
using namespace PlateSolver;

unsigned int get_longest_name_length(const std::string &csv_file_address)   {
    ifstream input_file (csv_file_address);
    unsigned int result = 0;
    string line;
    if (input_file.is_open())    {
        while ( getline (input_file,line) )        {
            StripString(&line);
            if (StartsWith(line, "#") || line.length() == 0)    {
                continue;
            }
            vector<string> elements = SplitString(line, ",");

            try {
                const string name = elements.at(1);
                if (name.length() > result) {
                    result = name.length();
                }
            }
            catch(...)  {
                throw std::string("Unable to read file \"" + csv_file_address + "\"");
            }

        }
        input_file.close();
    }
    else    {
        throw std::string("Unable to open file \"" + csv_file_address + "\"");
    }
    return result;
}


int main(int argc, const char **argv)   {
    try {
        if (argc != 3 && argc != 4)  {
            cout << "Two or three input arguments are required:\n";
            cout << "\t1st = catalogue of stars in csv\n";
            cout << "\t2nd = output binary catalogue of stars with RA, dec and mag only\n";
            cout << "\t3rd = optional. Binary file with names of the stars (will be used to annotate the photo)\n";
            return 0;
        }
        const string csv_file_address               = argv[1];
        const string output_binary_address_numbers  = argv[2];
        const string output_binary_address_names    = argc > 3 ? argv[3] : "";

        string line;
        ifstream input_file (csv_file_address);
        ofstream output_file_numbers(output_binary_address_numbers,std::ios::binary | std::ios::out);
        shared_ptr<ofstream> output_file_names = (output_binary_address_names.length()) ? make_shared<ofstream>(output_binary_address_names,std::ios::binary | std::ios::out) : nullptr;
        const unsigned int longest_name_length = output_file_names ? get_longest_name_length(csv_file_address) : 0;
        output_file_names->write(reinterpret_cast<const char *>(&longest_name_length), sizeof(longest_name_length));

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
                    string name = elements.at(1);
                    while (name.length() < longest_name_length) {
                        name = name + " ";
                    }

                    output_file_numbers.write(reinterpret_cast<const char *>(&RA),  sizeof(RA));
                    output_file_numbers.write(reinterpret_cast<const char *>(&dec), sizeof(dec));
                    output_file_numbers.write(reinterpret_cast<const char *>(&mag), sizeof(mag));
                    if (output_file_names)    {
                        output_file_names->write(name.c_str(), longest_name_length);
                    }
                }
                catch(...)  {
                    throw std::string("Unable to read file \"" + csv_file_address + "\"");
                }

            }
            input_file.close();
            output_file_numbers.close();
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