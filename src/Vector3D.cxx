#include "../PlateSolver/Vector3D.h"

#include <cmath>

using namespace PlateSolver;

Vector3D::Vector3D(float x, float y, float z, CoordinateSystem coordinate_system)    {
    if (coordinate_system == CoordinateSystem::enum_cartesian)  {
        m_x = x;
        m_y = y;
        m_z = z;
    }
    if (coordinate_system == CoordinateSystem::enum_spherical)  {
        m_x = x*cos(y)*cos(z);
        m_y = x*cos(y)*sin(z);
        m_z = x*sin(y);
    }
};

Vector3D Vector3D::get_coordinates_in_rotated_system(const Vector3D &rotated_z_axis)  const {
    const Vector3D rotated_x_axis = (Vector3D(0,1,0))*rotated_z_axis;
    const Vector3D rotated_y_axis = rotated_z_axis*(Vector3D(1,0,0));

    return Vector3D(scalar_product(rotated_x_axis), scalar_product(rotated_y_axis), scalar_product(rotated_z_axis));
};