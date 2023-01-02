#include "../PlateSolver/StringOperations.h"

#include <tuple>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;
using namespace PlateSolver;

int main(int argc, const char **argv)   {
    try {
        if (argc != 3)  {
            cout << "Three input arguments are required:\n";
            cout << "\t1st = address of the text file with asterisms hashes\n";
            cout << "\t2nd = address of the binary file with asterisms hashes\n";
            return 0;
        }

        const string text_file     = argv[1];
        const string binary_file   = argv[2];

        string line;
        ifstream input_file(text_file);
        ofstream output_file(binary_file,std::ios::binary | std::ios::out);
        if (input_file.is_open())    {
            while ( getline (input_file,line) )        {
                StripString(&line);
                if (StartsWith(line, "#") || line.length() == 0)    {
                    continue;
                }
                vector<string> elements = SplitString(line, ",");
                if (elements.size() != 8)   {
                    continue;
                }

                tuple<float,float,float,float> hash ({
                    std::stod(elements[0]),
                    std::stod(elements[1]),
                    std::stod(elements[2]),
                    std::stod(elements[3]),
                });

                unsigned int star_ids[4];
                star_ids[0] = std::stoi(elements[4]);
                star_ids[1] = std::stoi(elements[5]);
                star_ids[2] = std::stoi(elements[6]);
                star_ids[3] = std::stoi(elements[7]);

                output_file.write(reinterpret_cast<char *>(&hash), sizeof(hash));
                output_file.write(reinterpret_cast<char *>(star_ids), sizeof(hash));
            }
            input_file.close();
        }
        output_file.close();

        return 0;
    }
    catch(const string &e)  {
        cout << e << endl;
        abort();
    }
}