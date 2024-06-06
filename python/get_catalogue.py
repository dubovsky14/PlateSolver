"""
This script can be used to obtained csv file with coordinates of the stars in the night sky together with their magnitudes.
One input argument is required: address of the result csv file.
"""

from hypatie.simbad import object_type, sql2df
from sys import argv

if __name__ == "__main__":
    if not (len(argv) in [2,3]):
        print("The script requires one or two input arguments: \n1) the address of the output csv file (mandatory) \n2) the maximum magnitude (optional)")
        exit(1)

    max_magnitude = 11
    if len(argv) == 3:
        max_magnitude = float(argv[2])
    counter = 0

    interval_min, interval_max = -2,7
    step_width = 0.5
    with open(argv[1], "w") as output_csv:
        output_csv.write("#id,name,RA,dec,mag\n")
        while interval_max < max_magnitude:
            print(f"Reading interval of magnitudes: {interval_min} to {interval_max}")
            interval_width = interval_max-interval_min
            a = sql2df("SELECT main_id, V, ra/15, dec FROM basic JOIN allfluxes ON oid=oidref WHERE otype = 'star..' AND V BETWEEN "  +
                        str(interval_min) + " AND " + str(interval_max) + " ORDER BY V ASC")

            print(f"Found {len(a)} stars")
            if (len(a)) >= 50000:
                step_width *= 0.5
                interval_max = interval_min + step_width
                continue

            for i_row in range(0, len(a)):
                if (not a.loc[i_row]["DIV"] and not a.loc[i_row]["dec"]):
                    continue

                output_csv.write(str(counter) + ",")
                output_csv.write(a.loc[i_row]["main_id"] + ",")
                output_csv.write(a.loc[i_row]["DIV"] + ",")
                output_csv.write(a.loc[i_row]["dec"] + ",")
                output_csv.write(a.loc[i_row]["V"] + "\n")
                counter += 1

            interval_min = interval_max
            interval_max = interval_min + step_width


