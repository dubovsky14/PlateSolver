#include "../PlateSolver/StarFinder.h"


#include <vector>
#include <tuple>

using namespace PlateSolver;
using namespace std;

StarFinder::StarFinder(const std::vector<unsigned char> &pixels, unsigned int pixels_per_line)  {
    m_histogram.resize(256);
    m_pixels = pixels;
    fill_histogram();
};

// vector of tuples<x-position, y-position, intensity>
std::vector<std::tuple<float, float, float> >   StarFinder::get_stars() {

};

void StarFinder::reset_histogram()  {
    for (auto &x : m_histogram) x = 0;
};

void StarFinder::fill_histogram()    {
    for (unsigned char pixel : m_pixels)    {
        m_histogram[pixel]++;
    }
}

float   StarFinder::get_threshold(float part)   {
    const unsigned int n_birgther_pixels = part*m_pixels.size();
    unsigned int current_pixels = 0;

    for (unsigned int i_threshold = m_histogram.size() - 1; i_threshold != 0; i_threshold--)    {
        current_pixels += m_histogram[i_threshold];
        if (current_pixels > n_birgther_pixels) return i_threshold;
    }
    return 0;
};