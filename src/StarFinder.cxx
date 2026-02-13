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


std::vector< std::vector<std::tuple<unsigned int, unsigned int> > > StarFinder::get_clusters(float threshold)  const {
    std::vector<int> cluster_indices(m_width*m_height, -1);
    unsigned int n_clusters = 0;
    std::map<int, int> cluster_index_mapping;

    // The problem is that given pixel might not any top or left neighbor from known cluster, but as you go through the line, you might find a pixel with a neighboring cluster - we will take care of merging later
    auto add_cluster_mapping = [&cluster_index_mapping] (int from, int to) -> void {
        if (from < to) std::swap(from, to);
        if (cluster_index_mapping.find(from) != cluster_index_mapping.end())    {
            const int previosly_mapped_to = cluster_index_mapping[from];
            if (previosly_mapped_to > to) {
                cluster_index_mapping[from] = to;
            }
        }
        else {
            cluster_index_mapping[from] = to;
        }
    };

    auto get_cluster_index = [&cluster_indices, &n_clusters, this, &add_cluster_mapping] (unsigned int x, unsigned int y) -> int {
        int result = -1;
        // check pixel on the left
        if (x > 0) {
            if (cluster_indices[y*m_width + x-1] != -1) {
                result = cluster_indices[y*m_width + x-1];
            }
        }

        // check pixels on the top
        if (y > 0) {
            if (x > 0) {
                const int cluster_index = cluster_indices[(y-1)*m_width + x-1];
                if (cluster_index != -1) {
                    if (result != -1){
                        add_cluster_mapping(result, cluster_index);
                    }
                    result = result < 0 ? cluster_index : std::min(result, cluster_index);
                }
            }
            if (cluster_indices[(y-1)*m_width + x] != -1) {
                const int cluster_index = cluster_indices[(y-1)*m_width + x];
                if (result != -1) {
                    add_cluster_mapping(result, cluster_index);
                }
                    result = result < 0 ? cluster_index : std::min(result, cluster_index);
            }
            if (x < (m_width - 1)) {
                const int cluster_index = cluster_indices[(y-1)*m_width + x+1];
                if (cluster_index != -1) {
                    if (result != -1) {
                        add_cluster_mapping(result, cluster_index);
                    }
                    result = result < 0 ? cluster_index : std::min(result, cluster_index);
                }
            }
        }

        if (result != -1)   return result;

        // no neighboring pixels belong to a cluster, create a new cluster
        return n_clusters++;
    };

    for (unsigned int y_pos = 0; y_pos < m_height; y_pos++)    {
        for (unsigned int x_pos = 0; x_pos < m_width; x_pos++)    {
            if (read_pixel(x_pos, y_pos) < threshold)   continue;

            const int index = get_cluster_index(x_pos, y_pos);
            cluster_indices[y_pos*m_width + x_pos] = index;
        }
    }


    // calculate final mapping of cluster indices
    for (unsigned int i = 0; i < n_clusters; i++)    {
        if (cluster_index_mapping.find(i) != cluster_index_mapping.end())    {
            int mapped_to = cluster_index_mapping[i];
            while (cluster_index_mapping.find(mapped_to) != cluster_index_mapping.end())    {
                if (mapped_to == cluster_index_mapping[mapped_to] || mapped_to < cluster_index_mapping[mapped_to]) {
                    break;
                }
                mapped_to = cluster_index_mapping[mapped_to];
            }
            cluster_index_mapping[i] = mapped_to;
        }
        else {
            cluster_index_mapping[i] = i;
        }
    }

    std::vector< std::vector<std::tuple<unsigned int, unsigned int> > >  result(n_clusters);
    for (unsigned int y_pos = 0; y_pos < m_height; y_pos++)    {
        for (unsigned int x_pos = 0; x_pos < m_width; x_pos++)    {
            if (cluster_indices[y_pos*m_width + x_pos] != -1) {
                const int mapped_to = cluster_index_mapping[cluster_indices[y_pos*m_width + x_pos]];
                result[mapped_to].push_back(std::make_tuple(x_pos, y_pos));
            }
        }
    }

    std::sort(result.begin(), result.end(), [](const std::vector<std::tuple<unsigned int, unsigned int> > &a, const std::vector<std::tuple<unsigned int, unsigned int> > &b) {
        return a.size() > b.size();
    });

    return result;
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