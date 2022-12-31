#include "PlateSolver/StringOperations.h"

#include <algorithm>

using namespace std;

bool PlateSolver::StringIsInt(const string &x)    {
    try {
        std::stoi(x);
    }
    catch (...) {
        return false;
    }
    return true;
};

bool PlateSolver::StringIsFloat(const string &x)   {
    try {
        std::stod(x);
    }
    catch (...) {
        return false;
    }
    return true;
};

vector<string> PlateSolver::SplitAndStripString(const string &input_string, const string &separator) {
    vector<string> result = SplitString(input_string, separator);
    for (string &x : result)    {
        StripString(&x, " \n\t\r");
    }
    return result;
}

vector<string> PlateSolver::SplitString(string input_string, const string &separator)    {
    vector<string> result;
    size_t pos = 0;
    while ((pos = input_string.find(separator)) != std::string::npos) {
        result.push_back(input_string.substr(0, pos));
        input_string.erase(0, pos + separator.length());
    }
    if (input_string.length() > 0) result.push_back(input_string);
    return result;
};

std::vector<std::string> PlateSolver::SplitByWhitespaces(std::string line)   {
    const string white_space_chars = " \t\r\n";
    StripString(&line, white_space_chars);
    std::vector<std::string> result;
    while (line.size() > 0) {
        const std::size_t first_white_space = line.find_first_of(white_space_chars);
        if (first_white_space == std::string::npos) {
            result.push_back(line);
            break;
        }
        else    {
            result.push_back(line.substr(0, first_white_space));
            line.erase(0, first_white_space);
            StripString(&line, white_space_chars);
        }
    }
    return result;
};

void PlateSolver::StripString(string *input_string, const string &chars_to_remove)    {
    input_string->erase(0,input_string->find_first_not_of(chars_to_remove));
    input_string->erase(input_string->find_last_not_of(chars_to_remove)+1);
}

void         PlateSolver::to_upper(std::string *input_string) {
    std::transform(input_string->begin(), input_string->end(),input_string->begin(), ::toupper);
};

std::string  PlateSolver::to_upper_copy(const std::string &input_string)  {
    string result = input_string;
    PlateSolver::to_upper(&result);
    return result;
};

// convert both strings to uppercase and compare
bool PlateSolver::CompareCaseInsensitive(const std::string &x, const std::string &y)    {
    const string x_upper = PlateSolver::to_upper_copy(x);
    const string y_upper = PlateSolver::to_upper_copy(y);
    return x_upper == y_upper;
};

bool PlateSolver::Contains(const string &main_string, const string &substring)    {
    return (main_string.find(substring)) != std::string::npos;
};

bool PlateSolver::StartsWith(const string &main_string, const string &prefix)    {
    const unsigned int prefix_lenght = prefix.length();
    const unsigned int string_lenght = main_string.length();
    if (prefix_lenght > string_lenght) return false;
    return prefix == main_string.substr(0, prefix_lenght);
};

bool PlateSolver::EndsWith(const string &main_string, const string &suffix)    {
    const unsigned int suffix_lenght = suffix.length();
    const unsigned int string_lenght = main_string.length();
    if (suffix_lenght > string_lenght) return false;
    return suffix == main_string.substr(string_lenght-suffix_lenght, string_lenght);
};
