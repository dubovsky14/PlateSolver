import os
from math import tan

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
    result = result + str(int(60*angle_floating_point)) + "\'"
    angle_floating_point = 60*(angle_floating_point*60-int(angle_floating_point*60))
    result = result + str(round(angle_floating_point,1)) + "\""
    return result

def get_list_of_index_files(address_of_input_files : str) -> list:
    dir_list = os.listdir(address_of_input_files)
    result = []
    for file_name in dir_list:
        if (file_name.endswith(".kdtree")):
            result.append(file_name)
    return get_sorted_index_files(result)

def get_sorted_index_files(index_files : list) -> None:
    index_files_map = {}
    lengths = []
    for index_file in index_files:
        N = len(index_file)
        if not N in index_files_map:
            index_files_map[N] = []
            lengths.append(N)
        index_files_map[N].append(index_file)

    result = []
    lengths.sort()
    for N in lengths:
        index_files_map[N].sort()
        for index_file in index_files_map[N]:
            result.append(index_file)
    return result

def clean_up_temp(folder : str) -> None:
    files=os.listdir(folder)
    for item in files:
        if item.endswith(".jpg"):
            os.remove(folder + "/" + item)

def angular_width_to_effective_focal_length(angular_width_radians : float) -> float:
    return (36/2)/tan(angular_width_radians/2)

def convert_string_angle_to_float(string_angle : str) -> float:
    string_angle = string_angle.replace(",", ".")
    string_angle = string_angle.replace("h", "d")
    string_angle = string_angle.replace("°", "d")
    string_angle = string_angle.replace("'", "m")
    string_angle = string_angle.replace('"', "s")
    try:
        return float(string_angle)
    except:
        pass

    if (string_angle.count("d")) != 1:
        raise TypeError("Invalid input")

    elements = string_angle.split("d")
    result = float(elements[0])
    is_positive = result > 0
    if result == 0:
        is_positive = not ("-" in elements[0])
    result_sign = 1 if is_positive else -1

    if (len(elements) == 1):
        return result
    if (len(elements[1]) == 0):
        return result

    elements = elements[1].split("m")
    minutes = float(elements[0])
    result += result_sign*minutes/60

    if (len(elements) == 1):
        return result
    if (len(elements[1]) == 0):
        return result

    elements = elements[1].split('s')
    seconds = float(elements[0])
    result += result_sign*seconds/3600

    return result




def get_target_coordinates(ra_string : str, dec_string : str) -> tuple:
    """
    Returns tuple bool, float,float
    1st = valid coordinates?
    2nd = RA
    3rd = dec
    """
    if (not ra_string or not dec_string):
        return (False,0,0)

    ra_float = dec_float = 0
    try:
        ra_float = convert_string_angle_to_float(ra_string)
        dec_float = convert_string_angle_to_float(dec_string)
        return (True,ra_float,dec_float)
    except:
        return (False,0,0)

