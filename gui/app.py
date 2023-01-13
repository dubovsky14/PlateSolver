# run as:
# uwsgi --http-socket :9090 --wsgi-file run_localhost.py --master
# access the gui in browser at address: localhost:9090

from bottle import Bottle, view, request, static_file, abort, route
import os, sys
from timeit import default_timer as timer

sys.path.append("../python")
from plate_solving_wrapper import plate_solve, annotate_photo
from tools.helper_functions import convert_angle_to_string, get_list_of_index_files

app = Bottle()

@app.route('/')
@view('index')
def show_index():
    index_files = get_list_of_index_files("../data/")
    selected_index_file = request.query.index_file
    selected_index_file = selected_index_file if selected_index_file in index_files else ""

    context =   {
                    "index_files" : index_files,
                    "selected_index_file" : selected_index_file
                }
    return context

@app.route('/static/css/<filename:re:.*\.css>')
def server_static_css(filename):
    return static_file(filename, root='static/css')

@app.route('/upload', method='POST')
@view('show_result')
def do_upload():
    index_file = request.forms.get("index_file")
    upload     = request.files.get('upload')
    name, ext = os.path.splitext(upload.filename)
    if ext.upper() not in ('.JPG','.PNG','.JPEG'):
        return 'File extension not allowed.'
    UPLOAD_FOLDER = "temp/uploads/"
    FILE_ADDRESS = UPLOAD_FOLDER + "/" + upload.filename
    if os.path.exists(FILE_ADDRESS ):
        os.remove(FILE_ADDRESS )
    upload.save(UPLOAD_FOLDER) # appends upload.filename automatically

    time_start = timer()
    catalogue_file = "../data/catalogue.bin" if os.path.exists("../data/catalogue.bin" ) else "../data/catalogue.csv"
    plate_solving_result = plate_solve(catalogue_file, "../data/" + index_file, FILE_ADDRESS)
    time_end = timer()

    RA      = plate_solving_result[0]
    dec     = plate_solving_result[1]
    rot     = plate_solving_result[2]
    width   = plate_solving_result[3]
    height  = plate_solving_result[4]

    str_RA      = convert_angle_to_string(RA    , "h")     if plate_solving_result else None
    str_dec     = convert_angle_to_string(dec   )          if plate_solving_result else None
    str_rot     = convert_angle_to_string(rot   )          if plate_solving_result else None
    str_width   = convert_angle_to_string(width )          if plate_solving_result else None
    str_height  = convert_angle_to_string(height)          if plate_solving_result else None


    if plate_solving_result:
        success = plate_solving_result[4] != 0
    else:
        success = False

    ANNOTATE = True
    if (ANNOTATE):
        annotate_photo("../data/catalogue.bin", "../data/catalogue_names.bin",
                        FILE_ADDRESS, "../data/deep_sky_objects/", RA, dec, rot*(3.14159/180), width*(3.14159/180),
                        "temp/annotated_images/" + upload.filename, 1400)

    context = {
            "success" : success,
            "RA" :      str_RA,
            "dec" :     str_dec,
            "rot" :     str_rot,
            "width" :   str_width,
            "height" :  str_height,
            "time_to_platesolve"  : (time_end-time_start),
            "index_file" : index_file,
    }

    os.remove(FILE_ADDRESS )

    return context

if __name__ == "__main__":
    app.run()