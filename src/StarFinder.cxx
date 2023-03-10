#include "../PlateSolver/StarFinder.h"


#include <vector>
#include <tuple>
#include <map>
#include <iostream>
#include <algorithm>

using namespace PlateSolver;
using namespace std;
using namespace cv;


StarFinder::StarFinder(const std::string &photo_address)    {
    m_histogram.resize(256);
    Mat original_image = imread(photo_address);
    cvtColor(original_image, m_image, COLOR_BGR2GRAY);
    m_width = m_image.cols;
    m_height = m_image.rows;
    fill_histogram();
};

StarFinder::StarFinder(const cv::Mat &photo)    {
    m_histogram.resize(256);
    cvtColor(photo, m_image, COLOR_BGR2GRAY);
    m_width = m_image.cols;
    m_height = m_image.rows;
    fill_histogram();
};

// vector of tuples<x-position, y-position, intensity>
std::vector<std::tuple<float, float, float> >   StarFinder::get_stars(float threshold, bool invert_y_axis)  const {
    std::vector< std::vector<std::tuple<unsigned int, unsigned int> > > clusters = get_clusters(threshold);
    sort(clusters.begin(), clusters.end(), [](const auto &a, const auto &b) {return a.size() > b.size();}  );

    vector<std::tuple<float, float, float> > result;
    for (const std::vector<std::tuple<unsigned int, unsigned int> > &cluster : clusters)    {
        float x,y;
        calculate_center_of_cluster(&x,&y,cluster);
        if (invert_y_axis)  {
            y = -y;
        }
        result.push_back(tuple<float,float,float>(x,y,cluster.size()));
    }
    return result;
};

void StarFinder::reset_histogram()  {
    std::transform(m_histogram.begin(), m_histogram.end(), m_histogram.begin(), [](auto x){return 0;});
};

void StarFinder::fill_histogram()    {
    for (unsigned int y_pos = 0; y_pos < m_height; y_pos++)    {
        for (unsigned int x_pos = 0; x_pos < m_width; x_pos++)    {
            m_histogram[read_pixel(x_pos, y_pos)]++;
        }
    }
}

float   StarFinder::get_threshold(float part)   const   {
    const unsigned int n_birgther_pixels = part*(m_image.rows)*(m_image.cols);
    unsigned int current_pixels = 0;

    for (unsigned int i_threshold = m_histogram.size() - 1; i_threshold != 0; i_threshold--)    {
        current_pixels += m_histogram[i_threshold];
        if (current_pixels > n_birgther_pixels) return i_threshold;
    }
    return 0;
};

std::vector< std::vector<std::tuple<unsigned int, unsigned int> > > StarFinder::get_clusters(float threshold)   const  {
    vector< vector<tuple<unsigned int, unsigned int> > >  result;
    map<tuple<unsigned int, unsigned int>, char> visited_pixels;

    vector<tuple<unsigned int, unsigned int> >  this_cluster;
    for (unsigned int y_pos = 0; y_pos < m_height; y_pos++)    {
        for (unsigned int x_pos = 0; x_pos < m_width; x_pos++)    {
            fill_cluster(x_pos, y_pos, &this_cluster, threshold, &visited_pixels);
            if (this_cluster.size())    {
                result.push_back(this_cluster);
                this_cluster.clear();
            }
        }
    }
    return result;
};

void StarFinder::fill_cluster(  unsigned int x_pos, unsigned int y_pos,
                    std::vector<std::tuple<unsigned int, unsigned int> > *current_cluster,
                    float threshold, std::map<std::tuple<unsigned int, unsigned int>, char> *visited_pixels)    const {
    if (read_pixel(x_pos, y_pos) < threshold)   return;

    tuple<unsigned int, unsigned int> this_pixel(x_pos,y_pos);
    if (visited_pixels->find(this_pixel) != visited_pixels->end())    {
        return;
    }
    (*visited_pixels)[this_pixel] = 0;
    current_cluster->push_back(this_pixel);
    for (int y_shift = -1; y_shift <= 1; y_shift++) {
        int y_pos_new = y_pos + y_shift;
        if (y_pos_new < 0 || (unsigned int) y_pos_new >= m_height) continue;
        for (int x_shift = -1; x_shift <= 1; x_shift++) {
            int x_pos_new = x_pos + x_shift;
            if (x_pos_new < 0 || (unsigned int) x_pos_new >= m_width) continue;
            if (x_shift == 0 && y_shift ==0)    continue;
            fill_cluster(x_pos_new, y_pos_new, current_cluster, threshold, visited_pixels);
        }
    }
};

void StarFinder::calculate_center_of_cluster(   float  *x_pos, float  *y_pos,
                                                const std::vector<std::tuple<unsigned int, unsigned int> > &current_cluster)    {

    *x_pos = 0;
    *y_pos = 0;
    for (const std::tuple<unsigned int, unsigned int> &point : current_cluster) {
        *x_pos += get<0>(point);
        *y_pos += get<1>(point);
    }
    *x_pos /= current_cluster.size();
    *y_pos /= current_cluster.size();
};