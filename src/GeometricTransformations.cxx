#include "../PlateSolver/GeometricTransformations.h"

#include "../PlateSolver/Vector3D.h"
#include "../PlateSolver/Common.h"

#include <cmath>
#include <vector>
#include <tuple>
#include <string>

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


PixelCoordinatesToRaDecConvertor::PixelCoordinatesToRaDecConvertor( float center_RA, float center_dec, float rotation, float radians_per_pixel,
                                                                    float width_in_pixels, float height_in_pixels)  {

    m_x_axis = Vector3D::get_vector_unity_from_ra_dec(center_RA, center_dec);

    // let's rotate y-axis by "rotation" angle in z-y plane
    const Vector3D temp_y_axis = Vector3D(0, cos(rotation), sin(rotation));

    m_z_axis = m_x_axis*temp_y_axis;
    m_y_axis = m_z_axis*m_x_axis;

    m_half_height = height_in_pixels/2;
    m_half_width  = width_in_pixels/2;

    m_x_axis.normalize(radians_per_pixel);
    m_y_axis.normalize(radians_per_pixel);
    m_z_axis.normalize(radians_per_pixel);

};


std::tuple<float,float> PixelCoordinatesToRaDecConvertor::convert_to_ra_dec(float x, float y, ZeroZeroPoint zero_zero_point)  {
    if (zero_zero_point == ZeroZeroPoint::upper_left)   {
        x -= m_half_width;
        y += m_half_height;
    }

    Vector3D celestial_coordinates = m_x_axis*x + m_y_axis*y;
    const float xy_size = celestial_coordinates.r();
    if (xy_size > 1)    {
        throw string("convert_to_ra_dec:: invalid pixel coordinates");
    }

    const float coor_x = celestial_coordinates.x();
    const float coor_y = celestial_coordinates.y();
    const float coor_z = 1-sqrt(pow2(coor_x) + pow2(coor_y));

    celestial_coordinates = Vector3D(coor_x, coor_y, coor_z);

    return tuple<float,float>(celestial_coordinates.get_ra(), celestial_coordinates.get_dec());
};

RaDecToPixelCoordinatesConvertor::RaDecToPixelCoordinatesConvertor(   float center_RA, float center_dec, float rotation, float angle_per_pixel,
                                    float width_in_pixels, float height_in_pixels)  {

    m_x_axis = Vector3D::get_vector_unity_from_ra_dec(center_RA, center_dec);

    const Vector3D temp_y_axis = Vector3D(0, cos(-rotation), sin(-rotation));
    m_z_axis = m_x_axis*temp_y_axis;
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
    if (zero_zero_point == ZeroZeroPoint::center)   {
        return tuple<float,float>(position.scalar_product(m_y_axis), position.scalar_product(m_z_axis));
    }
    else    {
        return tuple<float,float>(position.scalar_product(m_y_axis)-m_half_width, position.scalar_product(m_z_axis)+m_half_height);
    }
};

