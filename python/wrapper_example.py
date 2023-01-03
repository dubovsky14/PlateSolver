# short example on how to use the plateSolving module

from sys import argv, path

# module "lives" in bin folder
path.append("../bin")


from plateSolving import plateSolving

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

if __name__ == "__main__":
    if (len(argv) != 4):
        print("Exactly 3 input arguments are expected:")
        print("\t1st = address of the csv file with catalogue of stars")
        print("\t2nd = address of the hash file")
        print("\t3rd = address of the photo to plate solve")

    star_catalogue = argv[1]
    hash_file      = argv[2]
    photo_address  = argv[3]

    plate_solving_result = plateSolving(star_catalogue, hash_file, photo_address)

    RA      = plate_solving_result[0]
    dec     = plate_solving_result[1]
    rot     = plate_solving_result[2]
    width   = plate_solving_result[3]
    height  = plate_solving_result[4]

    print("Plate solving finished.")
    print("RA = " + convert_angle_to_string(RA, "h"))
    print("dec = " + convert_angle_to_string(dec))
    print("rot = " + convert_angle_to_string(rot))
    print("photo covers the angle " + convert_angle_to_string(width) + " x " + convert_angle_to_string(height))