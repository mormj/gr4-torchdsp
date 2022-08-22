
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

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <gnuradio/types.h>
#include <numpy/arrayobject.h>

namespace py = pybind11;

#include <gnuradio/schedulers/triton/scheduler_triton.h>

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(scheduler_triton_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");
    using triton = gr::schedulers::scheduler_triton;
    py::class_<triton, gr::scheduler, std::shared_ptr<triton>>(m, "scheduler_triton")
        .def(py::init([](const std::string& name = "triton",
                         size_t default_buffer_size = 32768,
                         bool flush = true,
                         size_t flush_count = 8,
                         size_t flush_sleep_ms = 10) {
                 return ::gr::schedulers::scheduler_triton::make(
                     { name, default_buffer_size, flush, flush_count, flush_sleep_ms });
             }),
             py::arg("name") = "triton",
             py::arg("default_buffer_size") = 32768,
             py::arg("flush") = true,
             py::arg("flush_count") = 8,
             py::arg("flush_sleep_ms") = 10)
             ;

}
