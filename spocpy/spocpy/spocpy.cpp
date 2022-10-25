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

spoc::file::spoc_file py_readspocfile (const std::string &filename)
{
    // Open the file
    std::ifstream ifs (filename);
    if (!ifs)
        throw std::runtime_error ("Could not open file for reading");

    // Read the spoc file and return it
    return spoc::io::read_spoc_file (ifs);
}

void py_writespocfile (const std::string &filename, const spoc::file::spoc_file &f)
{
    // Open the file
    std::ofstream ofs (filename);
    if (!ofs)
        throw std::runtime_error ("Could not open file for writing");

    // Read the file and return it
    return spoc::io::write_spoc_file (ofs, f);
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

    py::class_<spoc::point_record::point_record>(m, "PointRecord")
        .def(py::init<>())
        .def_readwrite("x", &spoc::point_record::point_record::x)
        .def_readwrite("y", &spoc::point_record::point_record::y)
        .def_readwrite("z", &spoc::point_record::point_record::z)
        .def_readwrite("c", &spoc::point_record::point_record::c)
        .def_readwrite("p", &spoc::point_record::point_record::p)
        .def_readwrite("i", &spoc::point_record::point_record::i)
        .def_readwrite("r", &spoc::point_record::point_record::r)
        .def_readwrite("g", &spoc::point_record::point_record::g)
        .def_readwrite("b", &spoc::point_record::point_record::b)
        .def_readwrite("extra", &spoc::point_record::point_record::extra)
        ;

    py::class_<spoc::file::spoc_file>(m, "SpocFile")
        .def(py::init<const std::string &, const bool>())
        .def("getMajorVersion", &spoc::file::spoc_file::get_major_version)
        .def("getMinorVersion", &spoc::file::spoc_file::get_minor_version)
        .def("getWKT", &spoc::file::spoc_file::get_wkt)
        .def("setWKT", &spoc::file::spoc_file::set_wkt)
        .def("getCompressed", &spoc::file::spoc_file::get_compressed)
        .def("setCompressed", &spoc::file::spoc_file::set_compressed)
        .def("getPointRecord", &spoc::file::spoc_file::get_point_record)
        .def("setPointRecord", &spoc::file::spoc_file::set_point_record)
        .def("getPointRecords", &spoc::file::spoc_file::get_point_records)
        .def("setPointRecords", &spoc::file::spoc_file::set_point_records)
        .def("getX", &spoc::file::spoc_file::get_x)
        .def("getY", &spoc::file::spoc_file::get_y)
        .def("getZ", &spoc::file::spoc_file::get_z)
        .def("getC", &spoc::file::spoc_file::get_c)
        .def("getP", &spoc::file::spoc_file::get_p)
        .def("getI", &spoc::file::spoc_file::get_i)
        .def("getR", &spoc::file::spoc_file::get_r)
        .def("getG", &spoc::file::spoc_file::get_g)
        .def("getB", &spoc::file::spoc_file::get_b)
        .def("getExtra", &spoc::file::spoc_file::get_extra)
        ;
    m.def("readspocfile", &py_readspocfile, "Read a SPOC file");
    m.def("writespocfile", &py_writespocfile, "Write a SPOC file");
}
