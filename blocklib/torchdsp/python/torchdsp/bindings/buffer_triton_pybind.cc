/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/torchdsp/buffer_triton.h>
// pydoc.h is automatically generated in the build directory
// #include <edge_pydoc.h>

void bind_buffer_triton(py::module& m)
{
    using buffer_triton_properties = ::gr::buffer_triton_properties;

    py::enum_<gr::buffer_triton_type>(m, "buffer_triton_type")
        .value("H2D", gr::buffer_triton_type::H2D)
        .value("D2D", gr::buffer_triton_type::D2D)
        .value("D2H", gr::buffer_triton_type::D2H)
        .export_values();


    py::class_<buffer_triton_properties,
               gr::buffer_properties,
               std::shared_ptr<buffer_triton_properties>>(m, "buffer_triton_properties")
        .def_static("make",
                    &buffer_triton_properties::make,
                    py::arg("triton_url") = "localhost:8000",
                    py::arg("buffer_type") = gr::buffer_triton_type::D2D);
}
