#include "../PlateSolver/Vector3D.h"
#include "../PlateSolver/Common.h"

using namespace PlateSolver;
using namespace std;

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

Vector3D Vector3D::get_vector_unity_from_ra_dec(float RA, float dec)  {
    return Vector3D(1, dec*(M_PI/180), -RA*(M_PI/12), CoordinateSystem::enum_spherical);
};

float Vector3D::get_angle(const Vector3D &v1, const Vector3D &v2)   {
    return acos( (v1.scalar_product(v2))/(v1.r()*v2.r()) );
};

float Vector3D::get_ra()   const   {
    float RA = (-12/M_PI)*phi();
    if (RA < 24) RA+=24;
    return RA;
};

void Vector3D::normalize(float vector_size) {
    const float scale_factor = vector_size/r();
    m_x *= scale_factor;
    m_y *= scale_factor;
    m_z *= scale_factor;
};

std::string Vector3D::to_string(CoordinateSystem coordinate_system)  const {
    if (coordinate_system == CoordinateSystem::enum_cartesian)   {
        return ("[ " + std::to_string(m_x) + ", "  + std::to_string(m_y) + ", "  + std::to_string(m_z) + "]");
    }
    else {
        return ("[ " + std::to_string(r()) + ", "  + convert_to_deg_min_sec((180/M_PI)*theta()) + ", "  + convert_to_deg_min_sec((180/M_PI)*phi()) + "]");
    }
};