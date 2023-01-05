"""
python wrapper for the C++ part of the package
"""


from sys import path

# module "lives" in bin folder
path.append("../bin")

from plateSolving import plateSolving

def plate_solve(star_catalogue : str, hash_file : str, photo_address : str) -> tuple:
    """
    returns tuple of 5 floating point numbers: right ascension, declination, rotation of the camera,
    angular width of the photo, angular height of the photo
    """
    result = plateSolving(star_catalogue,hash_file, photo_address)
    if (type(result) == str):
        raise RuntimeError(result)
    if (result[4] == 0):
        return None
    return result

