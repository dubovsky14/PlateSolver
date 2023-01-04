#run as: uwsgi --http-socket :9090 --wsgi-file local_test.py --master

from bottle import Bottle, view, request, static_file, abort, route
import os, sys

sys.path.append("../python")
from plate_solving_wrapper import plate_solve
from tools.helper_functions import convert_angle_to_string

app = Bottle()

@app.route('/')
@view('index')
def show_index():
    context = {}

    return context


@app.route('/upload', method='POST')
@view('show_result')
def do_upload():
    upload     = request.files.get('upload')
    name, ext = os.path.splitext(upload.filename)
    if ext.upper() not in ('.JPG','.PNG','.JPEG'):
        return 'File extension not allowed.'
    UPLOAD_FOLDER = "temp/uploads/"
    FILE_ADDRESS = UPLOAD_FOLDER + "/" + upload.filename
    if os.path.exists(FILE_ADDRESS ):
        os.remove(FILE_ADDRESS )
    upload.save(UPLOAD_FOLDER) # appends upload.filename automatically

    plate_solving_result = plate_solve("../data/catalogue.csv", "../data/index_file_840mm.bin", FILE_ADDRESS)

    RA      = convert_angle_to_string(plate_solving_result[0], "h")
    dec     = convert_angle_to_string(plate_solving_result[1])
    rot     = convert_angle_to_string(plate_solving_result[2])
    width   = convert_angle_to_string(plate_solving_result[3])
    height  = convert_angle_to_string(plate_solving_result[4])

    context = {
            "RA" :      RA,
            "dec" :     dec,
            "rot" :     rot,
            "width" :   width,
            "height" :  height,
    }

    os.remove(FILE_ADDRESS )

    return context

if __name__ == "__main__":
    app.run()