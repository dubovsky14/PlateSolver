hd_catalogue = {}
hd_catalogue_initialized = False

def initialize_hd_catalogue(catalogue_address : str) -> tuple:
    global hd_catalogue
    global hd_catalogue_initialized
    hd_catalogue = {}
    try:
        with open(catalogue_address) as catalogue:
            for line in catalogue:
                if line.startswith("#"):
                    continue
                elements = line.split(",")
                if len(elements) != 5:
                    continue
                object_name = elements[1].upper().strip(' "')
                if not object_name.startswith("HD"):
                    continue

                star_number = object_name[2:].strip()

                try:
                    hd_catalogue[int(star_number)] =  (float(elements[2]), float(elements[3]))
                except:
                    pass
    except:
        RuntimeError("Catalogue does not exist")

    hd_catalogue_initialized = True


def find_star_hd_catalogue(catalogue_address : str, target_name : str) -> tuple:
    global hd_catalogue
    global hd_catalogue_initialized
    if not hd_catalogue_initialized:
        initialize_hd_catalogue(catalogue_address)

    target_name = target_name.upper().strip(' "')
    if not target_name.startswith("HD"):
        return (False,0,0)
    star_number = target_name[2:].strip()
    try:
        coordinates = hd_catalogue[int(star_number)]
        return (True, coordinates[0], coordinates[1])
    except:
        return (False,0,0)


def find_target_coordinates(catalogue_address : str, target_name : str) -> tuple:
    """
    returns:
    1st = bool - was target found?
    2nd = RA
    3nd = dec
    """

    target_name = target_name.upper().strip()
    candidates = []
    try:
        with open(catalogue_address) as catalogue:
            for line in catalogue:
                if line.startswith("#"):
                    continue
                elements = line.split(",")
                if len(elements) != 5:
                    continue
                object_name = elements[1].upper().strip()
                if object_name == target_name:
                    try:
                        return (True,float(elements[2]), float(elements[3]))
                    except:
                        return (False,0,0)
                else:
                    if (target_name in object_name):
                        candidates.append((object_name, float(elements[2]), float(elements[3])))
    except:
        pass

    # if exact match was not found, let's look at possible candidates:
    for candidate in candidates:
        if candidate[0].startswith(target_name + " "):
            return True,candidate[1],candidate[2]

    return (False,0,0)