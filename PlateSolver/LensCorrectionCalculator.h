#pragma once

#include <vector>
#include <string>
#include <utility>


namespace PlateSolver   {

    struct LensCorrectionCoefficients {
        double c_x = 0; // x-coordinate of lens' optical axis in pixels
        double c_y = 0; // y-coordinate of lens' optical axis in pixels
        double k1 = 0; // Term associated to r^2
        double k2 = 0; // Term associated to r^4
        double k3 = 0; // Term associated to r^6

        double sensor_half_diagonal_squared = 0; // Sensor half diagonal in pixels

        std::pair<double, double> get_undistorted_coordinates(double x, double y) const;
    };


    class LensCorrectionCalculator   {
        public:
            LensCorrectionCalculator(int n_coefficients = 2)    {
                m_n_coefficients = n_coefficients;
            };

            ~LensCorrectionCalculator() = default;

            LensCorrectionCoefficients calculate_corrections(   const std::string &photo_address,
                                                                const std::string &star_catalogue_file,
                                                                const std::string &hash_file)  const;

        private:
            int m_n_coefficients;

            std::vector<std::vector<double>> get_star_distance_matrix(const std::vector<std::pair<double, double>> &star_positions) const;

            std::vector<std::vector<double>> get_star_distance_matrix(const std::vector<std::pair<double, double>> &star_positions, const LensCorrectionCoefficients &lens_correction) const;

            double get_total_matrix_difference(const std::vector<std::vector<double>> &matrix_first, const std::vector<std::vector<double>> &matrix_second) const;
    };
}