"""
python wrapper for the C++ part of the package
"""


from sys import path

# module "lives" in bin folder
path.append("../bin")

from plateSolving import plateSolving, photoAnnotation, plateSolveAndAnnotate


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

def annotate_photo(star_catalogue_ra_dec : str, star_catalogue_names : str, original_photo_address : str, other_catalogues_folder : str,
                    ra_center : float, dec_center : float, rotation : float, angular_width : float,
                    output_photo_address : str, output_width_pixels : int) -> None:
    """
    Create small version of the input image with labelled stars and some deep sky objects and save it as "output_photo_address".
    If original picture is wider than "output_width_pixels", the output labelled picture will be down scaled to output_width_pixels width.
    """

    result = photoAnnotation(   star_catalogue_ra_dec, star_catalogue_names, original_photo_address,
                                other_catalogues_folder, output_photo_address,
                                ra_center, dec_center, rotation, angular_width, output_width_pixels)
    if (type(result) == str):
        raise RuntimeError(result)
    return result

def plate_solve_and_annotate_photo( hash_file : str, star_catalogue_ra_dec : str, star_catalogue_names : str, original_photo_address : str, other_catalogues_folder : str,
                                    output_photo_address : str, output_width_pixels : int) -> tuple:
    """
    It runs the plate-solving for the photo and create small annotated photo. The functionalities are exactly the same as when running plate_solve and annotate_photo functions one after another,
    however, this function is optimized for this case and certain operations, such as loading of input jpg, are performed only once, so that it's faster.
    """
    result = plateSolveAndAnnotate(hash_file, star_catalogue_ra_dec, star_catalogue_names, original_photo_address, other_catalogues_folder, output_photo_address, output_width_pixels)
    if (type(result) == str):
        raise RuntimeError(result)
    if (result[4] == 0):
        return None
    return result