from math import cos, sin
import numpy

def convert_equatorial_to_azimuthal(ra : float, dec : float, latitude : float, longitude : float, time_utc : float) -> tuple:
    """
    Returns azimuth and angular height in degrees
    """
    earth_rotation_period = 86164.0905
    reference_time = 1672531200

    # star equatorial coordinates of "up" direction on 1.1.2023, 0:00 from Trnava town square
    reference_star_ra = 6 + 51/60+39.9/3600
    Trnava_longitude = 17.58572

    #local sidereal time
    longitude_difference = Trnava_longitude - longitude
    sidereal_time = time_utc + (longitude_difference/360)*earth_rotation_period

    rotation_ra = 24*((sidereal_time-reference_time)%earth_rotation_period)/earth_rotation_period

    # equatorial coordinates of "up" direction in the current place and time
    up_ra = (reference_star_ra + rotation_ra)  % 24

    hour_angle_star = (up_ra - ra) % 24

    # target position in coordinates z = celestial north pole, x = (hour_angle = 0, dec = 0), y = (hour_angle = 6h, dec - 0)
    dec_rad = dec*3.14159/180
    hour_angle_rad = hour_angle_star*3.14159/12
    target_equatorial = numpy.array([cos(dec_rad)*cos(hour_angle_rad), cos(dec_rad)*sin(hour_angle_rad), sin(dec_rad)])

    # axes of hour angle and dec system in local coordinates (x -> north, y -> west, z -> up)
    x_axis = numpy.array([0, -cos(latitude*3.14159/180), sin(latitude*3.14159/180)])
    y_axis = numpy.array([1, 0, 0])
    z_axis = numpy.array([0, sin(latitude*3.14159/180), cos(latitude*3.14159/180)])

    # target coordinates in local coordinate system
    target_local = target_equatorial[0]*x_axis + target_equatorial[1]*y_axis + target_equatorial[2]*z_axis

    target_local_phi = numpy.arctan2(-target_local[0], target_local[1])
    target_local_theta = numpy.arcsin(target_local[2])

    return (target_local_phi*180/3.14159, target_local_theta*180/3.14159)

def get_3d_vector_from_azimuthal(azimuth : float, altitude : float) -> numpy.array:
    """
    Returns 3d vector from azimuth and altitude
    """
    azimuth_rad = azimuth*3.14159/180
    altitude_rad = altitude*3.14159/180
    return numpy.array([cos(altitude_rad)*cos(azimuth_rad), cos(altitude_rad)*sin(azimuth_rad), sin(altitude_rad)])

def get_angle(vec1 : numpy.array, vec2 : numpy.array) -> float:
    """
    Returns angle between two vectors in degrees
    """
    return numpy.arccos(numpy.dot(vec1, vec2)/(numpy.linalg.norm(vec1)*numpy.linalg.norm(vec2)))*180/3.14159


if __name__ == "__main__":
    # Merak
    ra = 11 + 3/60 + 15/3600
    dec = 56 + 15/60 + 22/3600
    #dec = 90
    latitude = 47.1490 # Eich
    longitude = 8.1551 # Eich
    #time_utc = 1675706182 # 6.2.2023, 18:56
    time_utc = 1675706182 - 3600*4.777

    north_pole = get_3d_vector_from_azimuthal(*convert_equatorial_to_azimuthal(ra, 90, latitude, longitude, time_utc))
    merak_now = get_3d_vector_from_azimuthal(*convert_equatorial_to_azimuthal(ra, dec, latitude, longitude, time_utc))
    merak_month_later = get_3d_vector_from_azimuthal(*convert_equatorial_to_azimuthal(ra, dec, latitude, longitude, time_utc + 3600*24*30))
    merak_10_years_later = get_3d_vector_from_azimuthal(*convert_equatorial_to_azimuthal(ra, dec, latitude, longitude, time_utc + 3600*24*365*10))

    sirius_now = get_3d_vector_from_azimuthal(*convert_equatorial_to_azimuthal(6 + 45/60 + 8.917/3600, -16 - 42/60 - 58.02/3600, latitude, longitude, time_utc))
    sirius_month_later = get_3d_vector_from_azimuthal(*convert_equatorial_to_azimuthal(6 + 45/60 + 8.917/3600, -16 - 42/60 - 58.02/3600, latitude, longitude, time_utc + 3600*24*30))

    print("Merak now: ", convert_equatorial_to_azimuthal(ra, dec, latitude, longitude, time_utc))
    print("Merak month later: ", convert_equatorial_to_azimuthal(ra, dec, latitude, longitude, time_utc + 3600*24*30))
    print("Merak 10 years later: ", convert_equatorial_to_azimuthal(ra, dec, latitude, longitude, time_utc + 3600*24*365*10))
    # 35d44m 28d2m # alt, az, hour angle around 15h