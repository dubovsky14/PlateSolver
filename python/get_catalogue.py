from hypatie.simbad import object_type, sql2df
#a = sql2df("SELECT main_id, V, ra/15, dec FROM basic JOIN allfluxes ON oid=oidref WHERE otype = 'star..' ORDER BY V ASC")
#result = sql2df("SELECT main_id, V, ra/15, dec FROM basic JOIN allfluxes ON oid=oidref WHERE otype = 'star..' AND V BETWEEN -2 AND 7 ORDER BY V ASC")

max_magnitude = 10
counter = 0

interval_min, interval_max = -2,7
step_width = 0.5
with open("catalogue.csv", "w") as output_csv:
    output_csv.write("#id,name,RA,dec,mag\n")
    while interval_max < 12:
        interval_width = interval_max-interval_min
        a = sql2df("SELECT main_id, V, ra/15, dec FROM basic JOIN allfluxes ON oid=oidref WHERE otype = 'star..' AND V BETWEEN "  +
                    str(interval_min) + " AND " + str(interval_max) + " ORDER BY V ASC")

        if (len(a)) >= 50000:
            step_width *= 0.5
            interval_max = interval_min + interval_width
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


