
def convert_angle_to_string(angle_floating_point : float, degree_symbol : str = "°") -> str:
    """
    convert angle from 3.6666666667 format to "3°40'0" format
    """
    result = ""
    if (angle_floating_point <0):
        result = "-"
        angle_floating_point *= -1

    result = result + str(int(angle_floating_point)) + degree_symbol
    angle_floating_point -= int(angle_floating_point)
    result = result + str(int(60*angle_floating_point)) + "\""
    angle_floating_point = 60*(angle_floating_point*60-int(angle_floating_point*60))
    result = result + str(round(angle_floating_point,1)) + "\'"
    return result