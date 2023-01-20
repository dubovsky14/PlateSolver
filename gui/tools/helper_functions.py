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
    result = result + str(int(60*angle_floating_point)) + "\""
    angle_floating_point = 60*(angle_floating_point*60-int(angle_floating_point*60))
    result = result + str(round(angle_floating_point,1)) + "\'"
    return result

def get_list_of_index_files(address_of_input_files) -> list:
    dir_list = os.listdir(address_of_input_files)
    result = []
    for file_name in dir_list:
        if not file_name.startswith("index_file_"):
            continue
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