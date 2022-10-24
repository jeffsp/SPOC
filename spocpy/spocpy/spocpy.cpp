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

spoc::header::header py_readheader (const std::string &filename)
{
    // Open the file
    std::ifstream ifs (filename);
    if (!ifs)
        throw std::runtime_error ("Could not open file for reading");

    // Read the header and return it
    return spoc::header::read_header (ifs);
}

//-------------------------
// Python module definition
//-------------------------

PYBIND11_MODULE(spocpy_cpp,m)
{
    m.doc() = "Spocpy C++ extension";

    m.def("getmajorversion", &py_getmajorversion, "Get the SPOC library major version number");
    m.def("getminorversion", &py_getminorversion, "Get the SPOC library minor version number");
    py::class_<spoc::header::header>(m, "Header")
        .def_readwrite("major_version", &spoc::header::header::major_version)
        .def_readwrite("minor_version", &spoc::header::header::minor_version)
        .def_readwrite("wkt", &spoc::header::header::wkt)
        .def_readwrite("extra_fields", &spoc::header::header::extra_fields)
        .def_readwrite("total_points", &spoc::header::header::total_points)
        .def_readwrite("compressed", &spoc::header::header::compressed)
        ;
    m.def("readheader", &py_readheader, "Read a SPOC file header");
}
