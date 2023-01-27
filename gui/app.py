# run as:
# uwsgi --http-socket :9090 --wsgi-file run_localhost.py --master
# access the gui in browser at address: localhost:9090

from bottle import Bottle, view, request, static_file, abort, route
import os, sys
from timeit import default_timer as timer

sys.path.append("../python")
from plate_solving_wrapper import plate_solve, annotate_photo, plate_solve_and_annotate_photo
import cpp_logging_wrapper
from tools.helper_functions import convert_angle_to_string, get_list_of_index_files, clean_up_temp, angular_width_to_effective_focal_length, get_target_coordinates

app = Bottle()
clean_up_temp("temp/annotated_images/")
cpp_logging_wrapper.set_log_file("temp/log.txt", False)
cpp_logging_wrapper.enable_logging()

@app.route('/')
@view('index')
def show_index():
    index_files = get_list_of_index_files("../data/")
    selected_index_file = request.query.index_file
    selected_index_file = selected_index_file if selected_index_file in index_files else ""

    target_ra = request.query.target_ra
    target_dec = request.query.target_dec

    if (not target_dec or not target_ra):
        target_ra =  ""
        target_dec = ""

    annotate_default = bool(request.query.annotate_default)
    print("Annotate default: " + str(annotate_default))

    context =   {
                    "index_files" : index_files,
                    "selected_index_file" : selected_index_file,
                    "annotate_default" : annotate_default,
                    "target_ra" : target_ra,
                    "target_dec" : target_dec,
                }
    return context

@app.route('/static/css/<filename:re:.*\.css>')
def server_static_css(filename):
    return static_file(filename, root='static/css')

@app.route('/temp/annotated_images/<filename:re:.*\.jpg>')
def server_static_jpg(filename):
    return static_file(filename, root='temp/annotated_images')

@app.route('/log')
def server_log():
    result = ""
    with open("temp/log.txt") as f:
        for line in f:
            result = result + line + "</br>"
    return result

@app.route('/upload', method='POST')
@view('show_result')
def do_upload():
    time_start = timer()
    cpp_logging_wrapper.log_message("\n\n")
    cpp_logging_wrapper.benchmark("Starting to process the request")
    valid_target,target_ra,target_dec = get_target_coordinates(request.forms.get("target_ra"), request.forms.get("target_dec"))
    index_file = request.forms.get("index_file")
    upload     = request.files.get('upload')
    name, ext = os.path.splitext(upload.filename)
    if ext.upper() not in ('.JPG','.PNG','.JPEG'):
        return 'File extension not allowed.'
    UPLOAD_FOLDER = "temp/uploads/"
    FILE_ADDRESS = UPLOAD_FOLDER + "/" + upload.filename
    cpp_logging_wrapper.log_message("Input photo: " + upload.filename)
    if os.path.exists(FILE_ADDRESS ):
        os.remove(FILE_ADDRESS )
    upload.save(UPLOAD_FOLDER) # appends upload.filename automatically

    cpp_logging_wrapper.benchmark("Data from request are loaded.")
    cpp_logging_wrapper.log_message("Going to use index file " + index_file)
    time_plate_solving_start = timer()
    catalogue_file = "../data/catalogue.bin" if os.path.exists("../data/catalogue.bin" ) else "../data/catalogue.csv"

    ANNOTATE = request.forms.get("checkbox_annotate")

    if (ANNOTATE):
        plate_solving_result = plate_solve_and_annotate_photo(  "../data/" + index_file,"../data/catalogue.bin", "../data/catalogue_names.bin",
                                                                FILE_ADDRESS, "../data/deep_sky_objects/",
                                                                "temp/annotated_images/" + name + ".jpg", 1400)
    else:
        plate_solving_result = plate_solve(catalogue_file, "../data/" + index_file, FILE_ADDRESS)
    time_plate_solving_end = timer()

    RA      = plate_solving_result[0] if plate_solving_result else None
    dec     = plate_solving_result[1] if plate_solving_result else None
    rot     = plate_solving_result[2] if plate_solving_result else None
    width   = plate_solving_result[3] if plate_solving_result else None
    height  = plate_solving_result[4] if plate_solving_result else None

    str_RA      = convert_angle_to_string(RA    , "h")     if plate_solving_result else None
    str_dec     = convert_angle_to_string(dec   )          if plate_solving_result else None
    str_rot     = convert_angle_to_string(rot   )          if plate_solving_result else None
    str_width   = convert_angle_to_string(width )          if plate_solving_result else None
    str_height  = convert_angle_to_string(height)          if plate_solving_result else None
    effective_focal_length = angular_width_to_effective_focal_length(max(width*3.14159/180,height*3.14159/180)) if plate_solving_result else None

    if plate_solving_result:
        success = plate_solving_result[4] != 0
    else:
        success = False

    os.remove(FILE_ADDRESS )
    time_end = timer()
    cpp_logging_wrapper.log_message("Overall time of processing the request: " + str(round(time_end-time_start, 3)))

    str_d_RA_target_photo = str_d_dec_target_photo = ""
    if (success and valid_target):
        str_d_dec_target_photo = ((target_dec-dec) > 0)*"+" + convert_angle_to_string(target_dec-dec)
        d_ra = target_ra-RA
        if (d_ra > 12):
            d_ra = d_ra-24
        if (d_ra < -12):
            d_ra = 24+d_ra

        str_d_RA_target_photo  = (d_ra > 0)*"+" + convert_angle_to_string(d_ra, "h")

    context = {
            "success" : success,
            "RA" :      str_RA,
            "dec" :     str_dec,
            "rot" :     str_rot,
            "width" :   str_width,
            "height" :  str_height,
            "time_to_platesolve"  : (time_plate_solving_end-time_plate_solving_start),
            "time_overall"  : (time_end-time_start),
            "index_file" : index_file,
            "annotated_photo" : "temp/annotated_images/" + name + ".jpg" if ANNOTATE else "",
            "effective_focal_length" : effective_focal_length,

            "valid_target"  : valid_target,
            "target_ra"     : convert_angle_to_string(target_ra,"h"),
            "target_dec"    : convert_angle_to_string(target_dec),
            "str_d_RA_target_photo"  : str_d_RA_target_photo,
            "str_d_dec_target_photo" : str_d_dec_target_photo,
    }

    return context

if __name__ == "__main__":
    app.run()