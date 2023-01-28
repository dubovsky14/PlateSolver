
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