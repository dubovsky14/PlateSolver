#include <Python.h>

#include <tuple>
#include <string>
#include <iostream>
#include <stdexcept>

#include "../PlateSolver/Logger.h"
#include "../PlateSolver/Common.h"

using namespace std;
using namespace PlateSolver;


static PyObject *benchmark_wrapper(PyObject *self, PyObject *args) {
    // Parse Input
    const char *char_message;
    if (!PyArg_ParseTuple(args, "s", &char_message))
    {
        return nullptr;
    };

    try{
        const string message = bench_mark(string(char_message));
        Logger::log_message(message);
        return Py_BuildValue("");
    }
    catch (const runtime_error &e) {
        const char *error = e.what();
        return Py_BuildValue("s", error);
    }
    return Py_BuildValue("");
}

static PyObject *log_message_wrapper(PyObject *self, PyObject *args) {
    // Parse Input
    const char *char_message;
    if (!PyArg_ParseTuple(args, "s", &char_message))
    {
        return nullptr;
    };

    try{
        Logger::log_message(string(char_message));
        return Py_BuildValue("");
    }
    catch (const runtime_error &e) {
        const char *error = e.what();
        return Py_BuildValue("s", error);
    }
    return Py_BuildValue("");
}

static PyObject *set_log_file_wrapper(PyObject *self, PyObject *args) {
    // Parse Input
    const char *char_log_address;
    int recreate;
    if (!PyArg_ParseTuple(args, "sp", &char_log_address, &recreate))
    {
        return nullptr;
    };

    try{
        Logger::set_log_file(string(char_log_address), recreate);
        return Py_BuildValue("");
    }
    catch (const runtime_error &e) {
        const char *error = e.what();
        return Py_BuildValue("s", error);
    }
    return Py_BuildValue("");
}

static PyObject *enable_logging_wrapper(PyObject *self, PyObject *args) {
    Logger::enable_logging();
    return Py_BuildValue("");
}

static PyObject *disable_logging_wrapper(PyObject *self, PyObject *args) {
    Logger::disable_logging();
    return Py_BuildValue("");
}

static PyMethodDef cppLoggerMethods[] = {
        {"log_message", log_message_wrapper, METH_VARARGS, " "},
        {"set_log_file", set_log_file_wrapper, METH_VARARGS, " "},
        {"enable_logging", enable_logging_wrapper, METH_VARARGS, " "},
        {"disable_logging", disable_logging_wrapper, METH_VARARGS, " "},
        {"benchmark", benchmark_wrapper, METH_VARARGS, " "},
        {nullptr, nullptr, 0, nullptr}};

static struct PyModuleDef cppLoggerModule = {
        PyModuleDef_HEAD_INIT,
        "cppLogger",
        nullptr,
        -1,
        cppLoggerMethods};

PyMODINIT_FUNC PyInit_cppLogger(void)
{
    return PyModule_Create(&cppLoggerModule);
}
