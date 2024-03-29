#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>

namespace PlateSolver {

    bool StringIsInt(const std::string &x);

    bool StringIsFloat(const std::string &x);

    std::vector<std::string> SplitAndStripString(const std::string &input_string, const std::string &separator);

    std::vector<std::string> SplitString(std::string input_string, const std::string &separator, unsigned int estimated_elements = 4);

    std::vector<std::string> SplitByWhitespaces(std::string line);

    void StripString(std::string *input_string, const std::string &chars_to_remove = " \n\t\r");

    void         to_upper(std::string *input_string);

    std::string  to_upper_copy(const std::string &input_string);

    // convert both strings to uppercase and compare
    bool CompareCaseInsensitive(const std::string &x, const std::string &y);

    bool Contains (const std::string &main_string, const std::string &substring);

    bool StartsWith(const std::string &main_string, const std::string &prefix);

    bool EndsWith(const std::string &main_string, const std::string &suffix);

    template <class ResultType>
    ResultType ConvertStringTo(const std::string &input_string) {
        throw std::runtime_error("Requested type not implemented!");
    };

    template <> inline
    int ConvertStringTo(const std::string &input_string)    {
        return std::stoi(input_string);
    };

    template <> inline
    unsigned long long int ConvertStringTo(const std::string &input_string)    {
        return std::stoull(input_string);
    };

    template <> inline
    float ConvertStringTo(const std::string &input_string)    {
        return std::stod(input_string);
    };

    template <> inline
    double ConvertStringTo(const std::string &input_string)    {
        return std::stod(input_string);
    };

    template <> inline
    std::string ConvertStringTo(const std::string &input_string)    {
        return input_string;
    };

    template <> inline
    bool ConvertStringTo(const std::string &input_string)    {
        const std::string input_upper = PlateSolver::to_upper_copy(input_string);
        if      (input_upper == "TRUE")  return true;
        else if (input_upper == "FALSE") return false;
        else {
            throw std::runtime_error("String \"" + input_string + "\" can't be converted to bool value!");
        }
    };

    template <> inline
    std::vector<int> ConvertStringTo(const std::string &input_string)    {
        std::string stripped_string = input_string;
        StripString(&stripped_string, "([{}})");
        std::vector<std::string> elements = SplitAndStripString(stripped_string, ",");

        std::vector<int> result;
        std::transform(elements.begin(),elements.end(),result.begin(), ConvertStringTo<int>);
        return result;
    };

    template <> inline
    std::vector<unsigned long long int> ConvertStringTo(const std::string &input_string)    {
        std::string stripped_string = input_string;
        StripString(&stripped_string, "([{}})");
        std::vector<std::string> elements = SplitAndStripString(stripped_string, ",");

        std::vector<unsigned long long int> result;
        std::transform(elements.begin(),elements.end(),result.begin(), ConvertStringTo<unsigned long long int>);
        return result;
    };
}
