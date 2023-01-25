#include <Python.h>

#include <tuple>
#include <string>
#include <iostream>

# include "../PlateSolver/PlateSolverTool.h"
# include "../PlateSolver/PhotoAnnotator.h"
#include "../PlateSolver/Logger.h"

#include<opencv4/opencv2/highgui/highgui.hpp>
#include<opencv4/opencv2/opencv.hpp>

using namespace std;
using namespace PlateSolver;

static PyObject *photoAnnotation_wrapper(PyObject *self, PyObject *args) {
    // Parse Input
    const char *char_star_catalogue_numbers, *char_star_catalogue_names, *char_photo_input;
    const char *char_other_catalogues_folder, *char_photo_output;
    float ra, dec, rot, angular_width;
    int image_width_pixels;

    if (!PyArg_ParseTuple(args, "sssssffffi",
                                        &char_star_catalogue_numbers,
                                        &char_star_catalogue_names,
                                        &char_photo_input,
                                        &char_other_catalogues_folder,
                                        &char_photo_output,
                                        &ra,
                                        &dec,
                                        &rot,
                                        &angular_width,
                                        &image_width_pixels
                                        ))
    {
        return nullptr;
    };

    try{

        const string star_catalogue_numbers(char_star_catalogue_numbers);
        const string star_catalogue_names(char_star_catalogue_names);
        const string photo_input(char_photo_input);
        const string other_catalogues_folder(char_other_catalogues_folder);
        const string photo_output(char_photo_output);

        PhotoAnnotator photo_annotator(star_catalogue_numbers, star_catalogue_names, other_catalogues_folder);
        photo_annotator.annotate_photo(photo_input, photo_output, image_width_pixels, ra, dec, rot, angular_width);

        return Py_BuildValue("");
    }
    catch (const string &e) {
        const char *error = e.c_str();
        return Py_BuildValue("s", error);
    }

    return Py_BuildValue("fffff", 0,0,0,0,0);
}

static PyObject *plateSolving_wrapper(PyObject *self, PyObject *args) {
    // Parse Input
    const char *char_catalogue, *char_hash_file, *char_photo;
    if (!PyArg_ParseTuple(args, "sss", &char_catalogue, &char_hash_file, &char_photo))
    {
        return nullptr;
    };

    try{
        const string star_catalogue(char_catalogue);
        const string hash_file(char_hash_file);
        const string photo_address(char_photo);

        PlateSolverTool plate_solver_tool(hash_file, star_catalogue);

        const tuple<float, float, float, float, float> result = plate_solver_tool.plate_solve(photo_address);
        const float RA = get<0>(result);
        const float dec = get<1>(result);
        const float rot = (180 / M_PI) * get<2>(result);
        const float width = (180 / M_PI) * get<3>(result);
        const float height = (180 / M_PI) * get<4>(result);

        return Py_BuildValue("fffff", RA, dec, rot, width, height);
    }
    catch (const string &e) {
        const char *error = e.c_str();
        return Py_BuildValue("s", error);
    }

    return Py_BuildValue("fffff", 0,0,0,0,0);
}

static PyObject *plate_solve_and_annotate_wrapper(PyObject *self, PyObject *args) {
    // Parse Input
    const char *char_hash_file, *char_star_catalogue_numbers, *char_star_catalogue_names, *char_photo_input;
    const char *char_other_catalogues_folder, *char_photo_output;
    int image_width_pixels;

    if (!PyArg_ParseTuple(args, "ssssssi",
                                        &char_hash_file,
                                        &char_star_catalogue_numbers,
                                        &char_star_catalogue_names,
                                        &char_photo_input,
                                        &char_other_catalogues_folder,
                                        &char_photo_output,
                                        &image_width_pixels
                                        ))
    {
        return nullptr;
    };

    try{


        const string hash_file(char_hash_file);
        const string star_catalogue_numbers(char_star_catalogue_numbers);
        const string star_catalogue_names(char_star_catalogue_names);
        const string photo_input_address(char_photo_input);
        const string other_catalogues_folder(char_other_catalogues_folder);
        const string photo_output(char_photo_output);

        PlateSolverTool plate_solver_tool(hash_file, star_catalogue_numbers);

        Logger::log_message(bench_mark("Going to read the photo."));
        cv::Mat photo = cv::imread(photo_input_address);
        Logger::log_message(bench_mark("Photo loaded."));

        const tuple<float, float, float, float, float> result = plate_solver_tool.plate_solve(photo);
        const float RA = get<0>(result);
        const float dec = get<1>(result);
        const float rot = (180 / M_PI) * get<2>(result);
        const float width = (180 / M_PI) * get<3>(result);
        const float height = (180 / M_PI) * get<4>(result);


        Logger::log_message(bench_mark("Plate solving finished. Going to annotate photo."));
        PhotoAnnotator photo_annotator(star_catalogue_numbers, star_catalogue_names, other_catalogues_folder);
        photo_annotator.annotate_photo(photo, photo_output, image_width_pixels, RA, dec, get<2>(result),get<3>(result));
        Logger::log_message(bench_mark("Photo annotation finished."));

        return Py_BuildValue("fffff", RA, dec, rot, width, height);
    }
    catch (const string &e) {
        const char *error = e.c_str();
        return Py_BuildValue("s", error);
    }

    return Py_BuildValue("fffff", 0,0,0,0,0);
}
static PyMethodDef plateSolvingMethods[] = {
        {"plateSolving", plateSolving_wrapper, METH_VARARGS, " "},
        {"photoAnnotation", photoAnnotation_wrapper, METH_VARARGS, " "},
        {"plateSolveAndAnnotate", plate_solve_and_annotate_wrapper, METH_VARARGS, " "},
        {nullptr, nullptr, 0, nullptr}};

static struct PyModuleDef plateSolvingModule = {
        PyModuleDef_HEAD_INIT,
        "plateSolving",
        nullptr,
        -1,
        plateSolvingMethods};

PyMODINIT_FUNC PyInit_plateSolving(void)
{
    return PyModule_Create(&plateSolvingModule);
}
