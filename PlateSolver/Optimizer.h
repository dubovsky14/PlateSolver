#pragma once

#include <functional>
#include <vector>
#include <tuple>

#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>

namespace PlateSolver {
    template<typename FloatingPointType>
    class Optimizer    {
        public:
            Optimizer() = delete;

            Optimizer(std::vector<FloatingPointType> *parameters, const std::vector<std::pair<FloatingPointType,FloatingPointType>> &limits)   {
                m_parameters = parameters;
                m_num_parameters = limits.size();
                if (m_parameters->size() != m_num_parameters) {
                    std::cerr << "Error: parameters and limits have different sizes\n";
                    exit(1);
                }

                m_limits = limits;
            };

            void reset_parameters(std::vector<FloatingPointType> *parameters)   {
                m_parameters = parameters;
            };

            void set_limits(const std::vector<std::pair<FloatingPointType,FloatingPointType>> &limits)   {
                m_limits = limits;
            };

            void set_learning_rate(float learning_rate) {
                m_learning_rate = learning_rate;
            };

            void set_decay_rate(float decay_rate) {
                m_decay_rate = decay_rate;
            };

            template<typename InputDataType>
            void run_optimization(  const std::function<FloatingPointType(
                                        const InputDataType *input_data_start,
                                        size_t samples_in_data,
                                        const FloatingPointType *parameters
                                    )> &objective_function,
                                    const InputDataType *input_data,
                                    size_t inputs_per_sample,
                                    size_t samples_in_data,
                                    size_t batch_size,
                                    unsigned int max_iterations = 1000) {

                m_deltas_for_gradient.clear();
                for (unsigned int i_param = 0; i_param < m_num_parameters; i_param++) {
                    const FloatingPointType delta_this_parameter = m_gradient_step*abs(m_limits[i_param].second - m_limits[i_param].first);
                    m_deltas_for_gradient.push_back(delta_this_parameter);
                }


                std::vector<InputDataType> input_data_buffer(samples_in_data * inputs_per_sample);
                for (size_t i_input = 0; i_input < samples_in_data * inputs_per_sample; i_input++) {
                    input_data_buffer[i_input] = input_data[i_input];
                }


                std::vector<FloatingPointType> gradient(m_num_parameters, 0);
                std::vector<FloatingPointType> second_derivative(m_num_parameters, 0);
                std::vector<FloatingPointType> accumulated_gradient = gradient;

                const float beta = 0.5;
                const size_t n_batches_per_iteration = (samples_in_data + batch_size - 1) / batch_size;
                std::vector<FloatingPointType> updated_parameters(m_num_parameters);
                for (size_t i_iter = 0; i_iter < max_iterations; i_iter++) {
                    m_i_iter = i_iter;

                    if (i_iter % 100 == 0)  {
                        std::cout << "Iteration #" << i_iter << std::endl << "\t Parameter values: ";
                        for (unsigned int i_param = 0; i_param < m_num_parameters; i_param++) {
                            std::cout << m_parameters->at(i_param) << " ";
                        }
                        std::cout << std::endl;

                        calculate_gradient_and_second_derivative<InputDataType>(
                            objective_function,
                            input_data_buffer.data(),
                            samples_in_data,
                            gradient.data(),
                            second_derivative.data()
                        );

                        std::cout << "\tgradient: ";
                        for (unsigned int i_param = 0; i_param < m_num_parameters; i_param++) {
                            std::cout << gradient.at(i_param) << " ";
                        }
                        std::cout << std::endl;

                        std::cout << "\t2nd derivative: ";
                        for (unsigned int i_param = 0; i_param < m_num_parameters; i_param++) {
                            std::cout << second_derivative.at(i_param) << " ";
                        }
                        std::cout << std::endl;

                        std::cout << "\tloss = " << objective_function(input_data_buffer.data(), samples_in_data, m_parameters->data()) << std::endl;

                    }

                    const std::vector<InputDataType> shuffled_inputs = shuffle_input_data(input_data_buffer, inputs_per_sample);
                    for (size_t i_batch = 0; i_batch < n_batches_per_iteration; i_batch++) {
                        size_t i_sample_batch_start = i_batch * batch_size;
                        size_t i_sample_batch_end = std::min(i_sample_batch_start + batch_size, samples_in_data);
                        const InputDataType *batch_data_start = &shuffled_inputs[i_sample_batch_start * inputs_per_sample];
                        const size_t samples_in_this_batch = i_sample_batch_end - i_sample_batch_start;

                        const FloatingPointType nominal_value = objective_function(
                            batch_data_start,
                            samples_in_this_batch,
                            m_parameters->data()
                        );

                        calculate_gradient_and_second_derivative<InputDataType>(
                            objective_function,
                            batch_data_start,
                            samples_in_this_batch,
                            gradient.data(),
                            second_derivative.data()
                        );

                        accumulated_gradient = add_vectors(accumulated_gradient, gradient, beta, 1-beta);

                        updated_parameters = *m_parameters;
                        for (unsigned int i_param = 0; i_param < m_num_parameters; i_param++) {
                            updated_parameters.at(i_param) -= m_learning_rate * accumulated_gradient[i_param];

                            if (updated_parameters.at(i_param) < m_limits[i_param].first) {
                                updated_parameters.at(i_param) = m_limits[i_param].first;
                            }
                            if (updated_parameters.at(i_param) > m_limits[i_param].second) {
                                updated_parameters.at(i_param) = m_limits[i_param].second;
                            }
                        }

                        const FloatingPointType updated_value = objective_function(batch_data_start, samples_in_this_batch, updated_parameters.data());

                        *m_parameters = updated_parameters;
                        m_learning_rate *= m_decay_rate;

                    }
                }
            };

            template <typename InputDataType>
            void calculate_gradient_and_second_derivative(
                    std::function<FloatingPointType(
                        const InputDataType *input_data_start,
                        size_t samples_in_data,
                        const FloatingPointType *parameters
                    )> objective_function,
                    const InputDataType *input_data_start,
                    size_t samples_in_data,
                    FloatingPointType *gradient,
                    FloatingPointType *second_derivative)  {


                const FloatingPointType nominal_value = objective_function(input_data_start, samples_in_data, m_parameters->data());
                for (unsigned int i_param = 0; i_param < m_num_parameters; i_param++) {

                    if (m_limits[i_param].second == m_limits[i_param].first)    {
                        gradient[i_param] = 0;
                        second_derivative[i_param] = 0;
                        continue;
                    }

                    FloatingPointType parameters_plus_delta[m_num_parameters];
                    FloatingPointType parameters_minus_delta[m_num_parameters];
                    for (unsigned int j = 0; j < m_num_parameters; j++) {
                        parameters_plus_delta[j] = m_parameters->at(j);
                        parameters_minus_delta[j] = m_parameters->at(j);
                    }

                    FloatingPointType &delta_this_parameter = m_deltas_for_gradient[i_param];

                    parameters_plus_delta[i_param] += delta_this_parameter;
                    parameters_minus_delta[i_param] -= delta_this_parameter;

                    const FloatingPointType value_plus = objective_function(input_data_start, samples_in_data, parameters_plus_delta);
                    const FloatingPointType value_minus = objective_function(input_data_start, samples_in_data, parameters_minus_delta);

                    gradient[i_param] = (value_plus - value_minus) / (2*delta_this_parameter);
                    second_derivative[i_param] = (value_plus - 2*nominal_value + value_minus) / (delta_this_parameter*delta_this_parameter);

                    //if (m_debug && (m_i_iter % 100 == 0))  {
                    //    std::cout << "\tParameter " << i_param << " " << m_parameters->at(i_param) << " (" << nominal_value << ")"  <<
                    //        "\t" << parameters_plus_delta[i_param] << " (" << value_plus << ")\t" <<
                    //        "\t" << parameters_minus_delta[i_param] << " (" << value_minus << ")\t" <<
                    //        "Gradient: " << gradient[i_param] << "\tSecond Derivative: " << second_derivative[i_param] << "\t"
                    //        << std::endl;
                    //}

                    if (value_plus == value_minus && value_plus == nominal_value)  {
                        delta_this_parameter *= 3;
                    }
                    else if (!((value_plus > nominal_value) ^ (value_minus > nominal_value)))  {
                        delta_this_parameter /= 3;
                    }
                }
            };

            void set_debug(bool debug)  { m_debug = debug; };

            void set_gradient_step(FloatingPointType step) { m_gradient_step = step; };

            std::vector<FloatingPointType> add_vectors( const std::vector<FloatingPointType> &a,
                                                        const std::vector<FloatingPointType> &b,
                                                        float coef_a = 1,
                                                        float coef_b = 1
                                                     ) {

                if (a.size() != b.size()) {
                    throw std::invalid_argument("Vectors must be of the same size");
                }

                std::vector<FloatingPointType> result(a.size());
                for (size_t i = 0; i < a.size(); ++i) {
                    result[i] = coef_a * a[i] + coef_b * b[i];
                }
                return result;
            };
        private:
            unsigned int m_num_parameters;
            std::vector<FloatingPointType> *m_parameters = nullptr;
            std::vector<std::pair<FloatingPointType,FloatingPointType>> m_limits;
            std::vector<FloatingPointType> m_deltas_for_gradient;

            float m_learning_rate = 0.01;
            float m_decay_rate = 0.9;

            static void normalize_vector(FloatingPointType *vector, unsigned int size) {
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
            int m_i_iter = 0;

            FloatingPointType m_gradient_step = 1e-4;

            template <typename T>
            std::vector<T> shuffle_input_data(const std::vector<T> &input, size_t numbers_per_sample) {
                if (input.size() % numbers_per_sample != 0) {
                    throw std::runtime_error("Inconsistent inputs in reshuffle_input_data");
                }
                const size_t n_samples = input.size() / numbers_per_sample;

                std::vector<T> reshuffled;
                reshuffled.reserve(input.size());
                std::vector<size_t> shuffled_sample_numbers(n_samples);
                for (size_t i = 0; i < n_samples; i++) {
                    shuffled_sample_numbers[i] = i;
                }

                for (const size_t i_sample : shuffled_sample_numbers)  {
                    for (size_t i_number_in_sample = 0; i_number_in_sample < numbers_per_sample; i_number_in_sample++)  {
                        reshuffled.push_back(input.at(i_sample*numbers_per_sample + i_number_in_sample));
                    }
                }
                return reshuffled;
            }

            std::default_random_engine m_random_generator;

    };
}