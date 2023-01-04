#pragma once

#include "../PlateSolver/Vector3D.h"

#include <vector>
#include <tuple>


namespace PlateSolver   {
    // return vector of 3 elements corresponding to rotated x,y and z axis, where X = reference axis and Z points to north celestial pole
    std::vector<Vector3D> get_rotated_axes(const Vector3D &reference_axis);

    enum class ZeroZeroPoint {center, upper_left};

    // Class responsible for conversion of pixel coordinates into RA-dec coordinates.
    // It takes RA and dec of the middle of the sensor, its rotation, size in pixels and angle covered by a pixel in its constructor and then provides a method for conversion
    class PixelCoordinatesToRaDecConvertor {
        public:

            PixelCoordinatesToRaDecConvertor() = delete;

            PixelCoordinatesToRaDecConvertor(   float center_RA, float center_dec, float rotation, float angle_per_pixel,
                                                float width_in_pixels = 0, float height_in_pixels = 0);

            // zero_zero_point = if set to upper_left, it will firstly transform coordinates to [(x-width_in_pixels/2), (y+height_in_pixels/2)]
            std::tuple<float,float> convert_to_ra_dec(float x, float y, ZeroZeroPoint zero_zero_point = ZeroZeroPoint::upper_left);


        private:
            Vector3D m_x_axis = Vector3D(1,0,0);
            Vector3D m_y_axis = Vector3D(0,1,0);
            Vector3D m_z_axis = Vector3D(0,0,1);

            float m_half_width, m_half_height;
            float m_angle_per_pixel;
            float m_rotation_matrix[2][2];

    };


    // Class responsible for conversion of RA-dec coordinates into pixel coordinates.
    // It takes RA and dec of the middle of the sensor, its rotation, size in pixels and angle covered by a pixel in its constructor and then provides a method for conversion
    class RaDecToPixelCoordinatesConvertor {
        public:

            RaDecToPixelCoordinatesConvertor() = delete;

            RaDecToPixelCoordinatesConvertor(   float center_RA, float center_dec, float rotation, float angle_per_pixel,
                                                float width_in_pixels = 0, float height_in_pixels = 0);

            std::tuple<float,float> convert_to_pixel_coordinates(float ra, float dec, ZeroZeroPoint zero_zero_point = ZeroZeroPoint::upper_left);

            std::tuple<float,float> convert_to_pixel_coordinates(const Vector3D &position, ZeroZeroPoint zero_zero_point = ZeroZeroPoint::upper_left);

        private:
            Vector3D m_x_axis = Vector3D(1,0,0);
            Vector3D m_y_axis = Vector3D(0,1,0);
            Vector3D m_z_axis = Vector3D(0,0,1);

            float m_half_width, m_half_height;
            float m_rotation_matrix[2][2];

    };
}