#include "../PlateSolver/LensCorrectionCalculator.h"

#include "../PlateSolver/PlateSolverTool.h"
#include "../PlateSolver/StarFinder.h"
#include "../PlateSolver/Fitter.h"

#include <map>

using namespace PlateSolver;
using namespace std;


std::pair<double, double> LensCorrectionCoefficients::get_undistorted_coordinates(double x, double y) const {
    const double x_distorted = x - c_x;
    const double y_distorted = y - c_y;

    const double r2 = (x_distorted * x_distorted + y_distorted * y_distorted) / sensor_half_diagonal_squared;
    const double r4 = r2 * r2;
    const double r6 = r4 * r2;

    const double radial_distortion = 1 + k1 * r2 + k2 * r4 + k3 * r6;

    const double x_undistorted = x_distorted * radial_distortion + c_x;
    const double y_undistorted = y_distorted * radial_distortion + c_y;

    return {x_undistorted, y_undistorted};
};

LensCorrectionCoefficients LensCorrectionCalculator::calculate_corrections( const std::string &photo_address,
                                                                            const std::string &star_catalogue_file,
                                                                            const std::string &hash_file)  const {
    PlateSolverTool plate_solver_tool(star_catalogue_file);

    const StarFinder star_finder(photo_address);
    const float threshold = star_finder.get_threshold(0.0005);
    std::vector<std::tuple<float, float, float>> star_positions = star_finder.get_stars(threshold);

    cout << "Detected " << star_positions.size() << " stars in the photo." << endl;
    const int n_stars_max = std::min<int>(30, star_positions.size());

    // sort by star size and keep only n_stars_max
    std::sort(star_positions.begin(), star_positions.end(), [](const std::tuple<float, float, float> &a, const std::tuple<float, float, float> &b) {
        return std::get<2>(a) > std::get<2>(b);
    });
    if (star_positions.size() > n_stars_max) {
        star_positions.resize(n_stars_max);
    }

    const int image_width = star_finder.get_width();
    const int image_height = star_finder.get_height();
    const float sensor_half_diagonal_squared = (image_width * image_width + image_height * image_height) / 4.0f;

    const std::tuple<float,float,float,float,float> plate_solver_result = plate_solver_tool.plate_solve(photo_address, hash_file);

    std::vector<std::pair<StarFromPhoto, StarFromDatabasePixelCoordinates> > paired_stars =  plate_solver_tool.get_paired_stars(
        star_positions,
        plate_solver_result,
        static_cast<float>(image_width),
        static_cast<float>(image_height),
        0.005
    );

    std::vector<std::pair<double, double>> paired_stars_from_photo;
    paired_stars_from_photo.reserve(paired_stars.size());
    for (const auto &pair : paired_stars) {
        paired_stars_from_photo.push_back({pair.first.x, pair.first.y});
    }

    std::vector<std::pair<double, double>> paired_stars_from_database;
    paired_stars_from_database.reserve(paired_stars.size());
    for (const auto &pair : paired_stars) {
        paired_stars_from_database.push_back({pair.second.x, pair.second.y});
    }

    cout << "In total " << paired_stars.size() << " star pairs have been identified." << endl;

    std::vector<double> coefficients({double(image_width/2), double(image_height/2)}); // cx, cy, k1, k2 (optional), k3 (optional)
    const int center_deviation = 60;
    std::vector<std::pair<double, double>> limits({
        {double(image_width/2 - center_deviation), double(image_width/2 + center_deviation)},
        {double(image_height/2 - center_deviation), double(image_height/2 + center_deviation)}
    });
    for (int i = 0; i < m_n_coefficients; ++i) {
        coefficients.push_back(0.0);
        limits.push_back({-0.1*pow(10, -2*i), 0.1*pow(10, -2*i)});
    }

    auto coef_vector_to_lens_correction = [sensor_half_diagonal_squared](const double *coefficients, size_t n_coefficients) -> LensCorrectionCoefficients {
        LensCorrectionCoefficients lens_correction;
        lens_correction.c_x = coefficients[0];
        lens_correction.c_y = coefficients[1];
        lens_correction.sensor_half_diagonal_squared = sensor_half_diagonal_squared;
        if (n_coefficients > 2) lens_correction.k1 = coefficients[2];
        if (n_coefficients > 3) lens_correction.k2 = coefficients[3];
        if (n_coefficients > 4) lens_correction.k3 = coefficients[4];
        return lens_correction;
    };

    std::map<int, vector<bool>> selection_masks; // key = iteration
    auto loss_function = [&selection_masks, &paired_stars_from_database, &coef_vector_to_lens_correction, &paired_stars_from_photo, this](const double *parameters, int seed) -> double {
        const LensCorrectionCoefficients lens_correction = coef_vector_to_lens_correction(parameters, m_n_coefficients + 2);
        if (selection_masks.find(seed) == selection_masks.end())    {
            int selected = 0;
            const float prob = 0.4;
            const float min_frac = 0.25;
            while (true)   {
                const vector<bool> selection_mask = get_random_batch_mask(paired_stars_from_photo.size(), prob);
                selected = 0;
                for (bool x : selection_mask)   {
                    selected += x;
                }
                if (selected > min_frac * paired_stars_from_photo.size())   {
                    selection_masks[seed] = selection_mask;
                    break;
                }
            }
        }
        const std::vector<bool> selection_mask = selection_masks[seed];
        vector<pair<double,double>> selected_stars_from_database = get_selected_elements(paired_stars_from_database, selection_mask);
        vector<pair<double,double>> selected_stars_from_photo = get_selected_elements(paired_stars_from_photo, selection_mask);


        const std::vector<std::vector<double>> distance_matrix_from_photo = get_star_distance_matrix(selected_stars_from_photo, lens_correction);
        const std::vector<std::vector<double>> distance_matrix_from_database = get_star_distance_matrix(selected_stars_from_database, lens_correction);

        return get_total_matrix_difference(distance_matrix_from_photo, distance_matrix_from_database);
    };


    Fitter<double> fitter(&coefficients, limits);
    fitter.set_debug(true);
    fitter.set_gradient_step(0.005);
    fitter.fit_gradient(loss_function, 0.1, 0.99, 10000);

    LensCorrectionCoefficients result;
    result.c_x = coefficients[0];
    result.c_y = coefficients[1];
    if (m_n_coefficients > 0) result.k1 = coefficients[2];
    if (m_n_coefficients > 1) result.k2 = coefficients[3];
    if (m_n_coefficients > 2) result.k3 = coefficients[4];
    result.sensor_half_diagonal_squared = sensor_half_diagonal_squared;
    return result;
};

std::vector<std::vector<double>> LensCorrectionCalculator::get_star_distance_matrix(const std::vector<std::pair<double, double>> &star_positions) const    {
    const size_t n_stars = star_positions.size();
    std::vector<std::vector<double>> distance_matrix(n_stars, std::vector<double>(n_stars, 0.0f));

    for (size_t i = 0; i < n_stars; ++i) {
        for (size_t j = i + 1; j < n_stars; ++j) {
            const double dx = star_positions[i].first - star_positions[j].first;
            const double dy = star_positions[i].second - star_positions[j].second;
            const double distance = std::sqrt(dx * dx + dy * dy);
            distance_matrix[i][j] = distance;
            distance_matrix[j][i] = distance;
        }
    }

    // normalize each row to 1
    for (size_t i = 0; i < n_stars; ++i) {
        double row_sum = 0;
        for (size_t j = 0; j < n_stars; ++j) {
            row_sum += distance_matrix[i][j];
        }
        if (row_sum > 0) {
            for (size_t j = 0; j < n_stars; ++j) {
                distance_matrix[i][j] /= row_sum;
            }
        }
    }

    return distance_matrix;
};

std::vector<std::vector<double>> LensCorrectionCalculator::get_star_distance_matrix( const std::vector<std::pair<double, double>> &star_positions,
                                                                                    const LensCorrectionCoefficients &lens_correction) const    {
    std::vector<std::pair<double,double>> undistorted_star_positions;
    undistorted_star_positions.reserve(star_positions.size());
    for (const auto &star_position : star_positions) {
        undistorted_star_positions.push_back(lens_correction.get_undistorted_coordinates(star_position.first, star_position.second));
    }
    return get_star_distance_matrix(undistorted_star_positions);
};

double LensCorrectionCalculator::get_total_matrix_difference(const std::vector<std::vector<double>> &matrix_first, const std::vector<std::vector<double>> &matrix_second) const  {
    const size_t n_stars = matrix_first.size();
    double total_difference = 0.0f;
    for (size_t i = 0; i < n_stars; ++i) {
        for (size_t j = 0; j < n_stars; ++j) {
            const double diff = matrix_first[i][j] - matrix_second[i][j];
            total_difference += diff * diff;
        }
    }
    return total_difference;
};

std::vector<bool> LensCorrectionCalculator::get_random_batch_mask(int n_elements_total, float prob) {
    vector<bool> result;
    for (int i_param = 0; i_param < n_elements_total; i_param++)  {
        const bool selected = ((float(rand())) / (float(RAND_MAX)+1)) < prob;
        result.push_back(selected);
    }
    return result;
};