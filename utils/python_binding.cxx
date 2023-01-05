#include <Python.h>

#include <tuple>
#include <string>
#include <iostream>

# include "../PlateSolver/PlateSolverTool.h"

using namespace std;
using namespace PlateSolver;

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

static PyMethodDef plateSolvingMethods[] = {
        {"plateSolving", plateSolving_wrapper, METH_VARARGS, " "},
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
