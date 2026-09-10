#pragma once

#include <functional>
#include <vector>
#include <tuple>

#include <cmath>
#include <iostream>

namespace PlateSolver {
    template<typename FlaotingPointType>
    class Fitter    {
        public:
            Fitter() = delete;

            Fitter(std::vector<FlaotingPointType> *parameters, const std::vector<std::pair<FlaotingPointType,FlaotingPointType>> &limits)   {
                m_parameters = parameters;
                m_num_parameters = limits.size();
                if (m_parameters->size() != m_num_parameters) {
                    std::cerr << "Error: parameters and limits have different sizes\n";
                    exit(1);
                }

                m_limits = limits;
            };

            void reset_parameters(std::vector<FlaotingPointType> *parameters)   {
                m_parameters = parameters;
            };

            void set_limits(const std::vector<std::pair<FlaotingPointType,FlaotingPointType>> &limits)   {
                m_limits = limits;
            };

            void fit_gradient(std::function<FlaotingPointType(const FlaotingPointType *parameters)> objective_function, FlaotingPointType learning_rate = 0.1, FlaotingPointType decay = 0.999, unsigned int max_iterations = 1000) {
                FlaotingPointType gradient[m_num_parameters];
                FlaotingPointType second_derivative[m_num_parameters];
                m_deltas_for_gradient.clear();
                for (unsigned int i_param = 0; i_param < m_num_parameters; i_param++) {
                    const FlaotingPointType delta_this_parameter = m_gradient_step*abs(m_limits[i_param].second - m_limits[i_param].first);
                    m_deltas_for_gradient.push_back(delta_this_parameter);
                }


                std::vector<FlaotingPointType> updated_parameters = *m_parameters;
                for (unsigned int i_iter = 0; i_iter < max_iterations; i_iter++) {
                    if (m_debug) {
                        std::cout << "Iteration " << i_iter << std::endl;
                    }
                    calculate_gradient_and_second_derivative(objective_function, gradient, second_derivative);
                    const FlaotingPointType nominal_value = objective_function(m_parameters->data());

                    updated_parameters = *m_parameters;
                    for (unsigned int i_param = 0; i_param < m_num_parameters; i_param++) {
                        updated_parameters.at(i_param) -= learning_rate*gradient[i_param];

                        if (updated_parameters.at(i_param) < m_limits[i_param].first) {
                            updated_parameters.at(i_param) = m_limits[i_param].first;
                        }
                        if (updated_parameters.at(i_param) > m_limits[i_param].second) {
                            updated_parameters.at(i_param) = m_limits[i_param].second;
                        }
                    }
                    const FlaotingPointType updated_value = objective_function(updated_parameters.data());
                    std::cout << "\tUpdated value: " << updated_value << " (Nominal value: " << nominal_value << ")" << "\tParameters: ";
                    for (unsigned int i_param = 0; i_param < m_num_parameters; i_param++) {
                        std::cout << updated_parameters.at(i_param) << "\t";
                    }
                    std::cout << "\tdeltas = ";
                    for (unsigned int i_param = 0; i_param < m_num_parameters; i_param++) {
                        std::cout << m_deltas_for_gradient.at(i_param) << "\t";
                    }

                    std::cout << std::endl;
                    if (updated_value < nominal_value) {
                        *m_parameters = updated_parameters;
                        learning_rate /= decay;
                    }
                    else {
                        learning_rate *= decay;
                    }
                }
            };

            void calculate_gradient_and_second_derivative(std::function<FlaotingPointType(const FlaotingPointType *parameters)> objective_function, FlaotingPointType *gradient, FlaotingPointType *second_derivative)  {
                const FlaotingPointType nominal_value = objective_function(m_parameters->data());
                for (unsigned int i_param = 0; i_param < m_num_parameters; i_param++) {
                    if (m_limits[i_param].second == m_limits[i_param].first)    {
                        gradient[i_param] = 0;
                        second_derivative[i_param] = 0;
                        continue;
                    }
                    FlaotingPointType parameters_plus_delta[m_num_parameters];
                    FlaotingPointType parameters_minus_delta[m_num_parameters];
                    for (unsigned int j = 0; j < m_num_parameters; j++) {
                        parameters_plus_delta[j] = m_parameters->at(j);
                        parameters_minus_delta[j] = m_parameters->at(j);
                    }

                    FlaotingPointType &delta_this_parameter = m_deltas_for_gradient[i_param];

                    parameters_plus_delta[i_param] += delta_this_parameter;
                    parameters_minus_delta[i_param] -= delta_this_parameter;

                    const FlaotingPointType value_plus = objective_function(parameters_plus_delta);
                    const FlaotingPointType value_minus = objective_function(parameters_minus_delta);

                    gradient[i_param] = (value_plus - value_minus) / (2*delta_this_parameter);
                    second_derivative[i_param] = (value_plus - 2*nominal_value + value_minus) / (delta_this_parameter*delta_this_parameter);

                    if (m_debug)  {
                        std::cout << "\tParameter " << i_param << " " << m_parameters->at(i_param) << " (" << nominal_value << ")"  <<
                            "\t" << parameters_plus_delta[i_param] << " (" << value_plus << ")\t" <<
                            "\t" << parameters_minus_delta[i_param] << " (" << value_minus << ")\t" <<
                            "Gradient: " << gradient[i_param] << "\tSecond Derivative: " << second_derivative[i_param] << "\t"
                            << std::endl;
                    }

                    if (value_plus == value_minus && value_plus == nominal_value)  {
                        delta_this_parameter *= 10;
                    }
                    else if (!((value_plus > nominal_value) ^ (value_minus > nominal_value)))  {
                        delta_this_parameter *= 0.1;
                    }
                }
            };

            void set_debug(bool debug)  { m_debug = debug; };

            void set_gradient_step(FlaotingPointType step) { m_gradient_step = step; };

        private:
            unsigned int m_num_parameters;
            std::vector<FlaotingPointType> *m_parameters = nullptr;
            std::vector<std::pair<FlaotingPointType,FlaotingPointType>> m_limits;
            std::vector<FlaotingPointType> m_deltas_for_gradient;

            static void normalize_vector(FlaotingPointType *vector, unsigned int size) {
                double norm = 0;
                for (unsigned int i = 0; i < size; i++) {
                    norm += vector[i]*vector[i];
                }
                norm = sqrt(norm);
                for (unsigned int i = 0; i < size; i++) {
                    vector[i] /= norm;
                }
            };

            bool m_debug = false;

            FlaotingPointType m_gradient_step = 1e-4;

    };
}