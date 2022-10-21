#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <vector>
#include "spoc/spoc.h"

//-----------------
// Python interface
//-----------------

namespace py = pybind11;

int py_getmajorversion ()
{
    return spoc::MAJOR_VERSION;
}

int py_getminorversion ()
{
    return spoc::MINOR_VERSION;
}

//-------------------------
// Python module definition
//-------------------------

PYBIND11_MODULE(spocpy_cpp,m)
{
    m.doc() = "Spocpy C++ extension";

    m.def("getmajorversion", &py_getmajorversion, "Get the library major version number");
    m.def("getminorversion", &py_getminorversion, "Get the library minor version number");
}
