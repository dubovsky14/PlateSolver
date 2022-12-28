#pragma once

#include <cmath>

namespace PlateSolver   {
    enum class CoordinateSystem {enum_cartesian, enum_spherical};

    class Vector3D  {
        public:
            // for cartesian (x,y,z), for spherical (r,theta from equator,phi)
            Vector3D(float x, float y, float z, CoordinateSystem coordinate_system = CoordinateSystem::enum_cartesian);

            // it's up to user's responsibility that |rotated_z_axis| = 1
            Vector3D get_coordinates_in_rotated_system(const Vector3D &rotated_z_axis)  const;

            inline Vector3D operator*(float b) const {
                return Vector3D(m_x*b, m_y*b, m_z*b);
            }

            inline Vector3D operator/(float b) const {
                return Vector3D(m_x/b, m_y/b, m_z/b);
            }

            inline Vector3D operator*(const Vector3D &b) const    {
                return Vector3D (
                    m_y*b.m_z - m_z*b.m_y,
                    m_z*b.m_x - m_x*b.m_z,
                    m_x*b.m_y - m_y*b.m_x
                );
            };

            inline float scalar_product(const Vector3D &b)  const    {
                return m_x*b.m_x + m_y*b.m_y +  m_z*b.m_z;
            };

            inline float x()   const {return m_x;};
            inline float y()   const {return m_y;};
            inline float z()   const {return m_z;};

            inline float r2()       const {return m_x*m_x + m_y*m_y + m_z*m_z;};
            inline float r()        const {return sqrt(r2());};
            inline float theta()    const {return asin(m_z/r());};
            inline float phi()      const {return acos(m_x/sqrt(m_x*m_x + m_y*m_y));};

            inline float get_ra()   const   {return (-12/M_PI)*phi();}; // TODO
            inline float get_dec()  const   {return (180/M_PI)*theta();};

        private:
            float m_x = 0;
            float m_y = 0;
            float m_z = 0;
    };
}