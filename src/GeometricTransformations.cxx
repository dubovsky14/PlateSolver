#include "../PlateSolver/GeometricTransformations.h"

#include "../PlateSolver/Vector3D.h"
#include "../PlateSolver/Common.h"

#include <cmath>
#include <vector>
#include <tuple>
#include <string>
#include <iostream>

using namespace PlateSolver;
using namespace std;

std::vector<Vector3D> PlateSolver::get_rotated_axes(const Vector3D &reference_axis)  {
    const Vector3D rotated_x_axis = reference_axis;
    const Vector3D rotated_y_axis = (reference_axis.x() == 0 && reference_axis.y() == 0) ?
                                    Vector3D(1,0,0) :   // if reference axis is z-axis (it's arbitrary choice, so I took x-axis)
                                    (Vector3D(0,0,1)*reference_axis);
    const Vector3D rotated_z_axis = rotated_x_axis*rotated_y_axis;

    return vector<Vector3D>{rotated_x_axis, rotated_y_axis, rotated_z_axis};
};


PixelCoordinatesToRaDecConvertor::PixelCoordinatesToRaDecConvertor( float center_RA, float center_dec, float rotation, float angle_per_pixel,
                                                                    float width_in_pixels, float height_in_pixels)  {


    m_rotation_matrix[0][0] = cos(rotation);
    m_rotation_matrix[0][1] = -sin(rotation);
    m_rotation_matrix[1][0] = -m_rotation_matrix[0][1]; // sin
    m_rotation_matrix[1][1] = m_rotation_matrix[0][0]; // cos

    m_x_axis = Vector3D::get_vector_unity_from_ra_dec(center_RA, center_dec);

    float z_axis_dec = center_dec + 90;
    float z_axis_RA = center_RA;
    if (z_axis_dec > 90)    {
        z_axis_dec = 180 - z_axis_dec;
        z_axis_RA += 12;
        if (z_axis_RA > 24) z_axis_RA -= 24;
    }
    m_z_axis = Vector3D::get_vector_unity_from_ra_dec(z_axis_RA, z_axis_dec);
    m_y_axis = m_z_axis*m_x_axis;

    m_half_width    = width_in_pixels/2;
    m_half_height   = height_in_pixels/2;

    m_x_axis.normalize(1.);
    m_y_axis.normalize(1.);
    m_z_axis.normalize(1.);
    m_angle_per_pixel = angle_per_pixel;
};


std::tuple<float,float> PixelCoordinatesToRaDecConvertor::convert_to_ra_dec(float x, float y, ZeroZeroPoint zero_zero_point)  {
    if (zero_zero_point == ZeroZeroPoint::upper_left)   {
        x -= m_half_width;
        y += m_half_height;
    }

    // firstly rotate it the way that "up" points to north celestial pole
    const float x_rot = x*m_rotation_matrix[0][0] + y*m_rotation_matrix[0][1];
    const float y_rot = x*m_rotation_matrix[1][0] + y*m_rotation_matrix[1][1];

    const float x_spherical = x_rot*m_angle_per_pixel;
    const float y_spherical = y_rot*m_angle_per_pixel;
    const float z_spherical = sqrt(1-pow2(x_spherical)- pow2(y_spherical));

    Vector3D celestial_coordinates = m_x_axis*z_spherical + m_y_axis*x_spherical + m_z_axis*y_spherical;

    return tuple<float,float>(celestial_coordinates.get_ra(), celestial_coordinates.get_dec());
};

RaDecToPixelCoordinatesConvertor::RaDecToPixelCoordinatesConvertor(   float center_RA, float center_dec, float rotation, float angle_per_pixel,
                                    float width_in_pixels, float height_in_pixels)  {

    m_rotation_matrix[0][0] = cos(rotation);
    m_rotation_matrix[0][1] = -sin(rotation);
    m_rotation_matrix[1][0] = -m_rotation_matrix[0][1]; // sin
    m_rotation_matrix[1][1] = m_rotation_matrix[0][0]; // cos

    m_x_axis = Vector3D::get_vector_unity_from_ra_dec(center_RA, center_dec);

    float z_axis_dec = center_dec + 90;
    float z_axis_RA = center_RA;
    if (z_axis_dec > 90)    {
        z_axis_dec = 180 - z_axis_dec;
        z_axis_RA += 12;
        if (z_axis_RA > 24) z_axis_RA -= 24;
    }
    m_z_axis = Vector3D::get_vector_unity_from_ra_dec(z_axis_RA, z_axis_dec);
    m_y_axis = m_z_axis*m_x_axis;

    m_x_axis.normalize(1/angle_per_pixel);
    m_y_axis.normalize(1/angle_per_pixel);
    m_z_axis.normalize(1/angle_per_pixel);


    m_half_width    = width_in_pixels/2;
    m_half_height   = height_in_pixels/2;
};

std::tuple<float,float> RaDecToPixelCoordinatesConvertor::convert_to_pixel_coordinates(float ra, float dec, ZeroZeroPoint zero_zero_point) {
    const Vector3D star_vector = Vector3D::get_vector_unity_from_ra_dec(ra, dec);
    return convert_to_pixel_coordinates(star_vector, zero_zero_point);
};

std::tuple<float,float> RaDecToPixelCoordinatesConvertor::convert_to_pixel_coordinates(const Vector3D &position, ZeroZeroPoint zero_zero_point)    {
    const float original_coordinates[2] = {position.scalar_product(m_y_axis), position.scalar_product(m_z_axis)};
    const float rotated_coordinates[] = {
        original_coordinates[0]*m_rotation_matrix[0][0] + original_coordinates[1]*m_rotation_matrix[0][1],
        original_coordinates[0]*m_rotation_matrix[1][0] + original_coordinates[1]*m_rotation_matrix[1][1]
    };
    if (zero_zero_point == ZeroZeroPoint::upper_left)   {
        return tuple<float,float>(
            rotated_coordinates[0] + m_half_width,
            rotated_coordinates[1] - m_half_height
        );
    }
    return tuple<float,float>(
        rotated_coordinates[0],
        rotated_coordinates[1]
    );
};

