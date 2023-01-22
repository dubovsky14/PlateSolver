"""
Plate solve the photo and compare the result with the sample result.
Four arguments are required:
1) address of the star catalogue
2) address of the hash file
3) address of the photo
4) result to compare with in form "RA,dec,rot,angular_width,angular_height"
"""

from sys import path, argv, exit
from math import cos, sin, acos

path.append("../bin")
path.append("../python")

def dphi(phi1 : float, phi2 : float) -> float:
    d_phi = abs(phi1-phi2) % 360
    if d_phi < 180:
        return d_phi
    else:
        return 360-d_phi


from plate_solving_wrapper import plate_solve

if __name__ == "__main__":
    if (len(argv)!=5):
        print("Four input arguments are required:")
        print("1) address of the star catalogue")
        print("2) address of the hash file")
        print("3) address of the photo")
        print('4) result to compare with in form "RA,dec,rot,angular_width,angular_height"')
        exit(1)

    plate_solving_result = plate_solve(argv[1], argv[2], argv[3])

    #parse sample result from 4th terminal input:
    elements = argv[4].split(",")
    result_correct = [float(x) for x in elements]

    dec_calc = plate_solving_result[1]*3.14159/180
    ra_calc = plate_solving_result[0]*3.14159/12
    center_vector_calculated = [cos(dec_calc), sin(dec_calc)*cos(ra_calc), sin(dec_calc)*sin(ra_calc)]

    dec_correct = result_correct[1]*3.14159/180
    ra_correct = result_correct[0]*3.14159/12
    center_vector_correct= [cos(dec_correct), sin(dec_correct)*cos(ra_correct), sin(dec_correct)*sin(ra_correct)]

    scalar_product = 0
    for i in range(0,3):
        scalar_product += center_vector_calculated[i]*center_vector_correct[i]
    angular_deviation_center_position = acos(scalar_product)

    if (angular_deviation_center_position > 0.1*result_correct[3]*3.15169/180):
        print("The position of the center do not match:")
        print("RA calculated = " + str(plate_solving_result[0]), " , RA correct = " + str(result_correct[0]))
        print("dec calculated = " + str(plate_solving_result[1]), " , dec correct = " + str(result_correct[1]))
        exit(1)

    if (dphi(plate_solving_result[2],result_correct[2]) > 1):
        print(abs((360 + plate_solving_result[2]-result_correct[2]) % 360))
        print("The camera rotation do not match the value from terminal:")
        print("rot calculated = " + str(plate_solving_result[2]), " , rot correct = " + str(result_correct[2]))
        exit(1)

    if (abs(plate_solving_result[3]-result_correct[3]) > 0.1*plate_solving_result[3]):
        print("The angular width do not match the value from terminal:")
        print("width calculated = " + str(plate_solving_result[3]), " , width correct = " + str(result_correct[3]))
        exit(1)