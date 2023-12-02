#pragma once

#include <cmath>
#include <string>

namespace PlateSolver   {
    enum class CoordinateSystem {enum_cartesian, enum_spherical};

    class Vector3D  {
        public:
            Vector3D() = default;

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

            inline Vector3D operator-(const Vector3D &b) const    {
                return Vector3D (
                    m_x-b.m_x,
                    m_y-b.m_y,
                    m_z-b.m_z
                );
            };

            inline Vector3D operator+(const Vector3D &b) const    {
                return Vector3D (
                    m_x+b.m_x,
                    m_y+b.m_y,
                    m_z+b.m_z
                );
            };

            inline float scalar_product(const Vector3D &b)  const    {
                return m_x*b.m_x + m_y*b.m_y +  m_z*b.m_z;
            };

            static Vector3D get_vector_unity_from_ra_dec(float RA, float dec);

            static float get_angle(const Vector3D &v1, const Vector3D &v2);

            inline float x()   const {return m_x;};
            inline float y()   const {return m_y;};
            inline float z()   const {return m_z;};

            inline float r2()       const {return m_x*m_x + m_y*m_y + m_z*m_z;};
            inline float r()        const {return sqrt(r2());};
            inline float theta()    const {return asin(m_z/r());};
            inline float phi()      const {return m_y > 0 ? acos(m_x/sqrt(m_x*m_x + m_y*m_y)) : (2*M_PI)-acos(m_x/sqrt(m_x*m_x + m_y*m_y));};

            inline float phi_mpi_pi()   const {return acos(m_x/sqrt(m_x*m_x + m_y*m_y));};

            float get_ra()   const;
            inline float get_dec()  const   {return (180/M_PI)*theta();};


            std::string to_string(CoordinateSystem coordinate_system = CoordinateSystem::enum_cartesian)  const;

            void normalize(float vector_size = 1);

        private:
            float m_x = 0;
            float m_y = 0;
            float m_z = 0;
    };
}