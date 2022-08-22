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

#include <gnuradio/torchdsp/triton_async_block.h>
// pydoc.h is automatically generated in the build directory
// #include <edge_pydoc.h>

void bind_triton_async_block(py::module& m)
{
    using async_block_interface = ::gr::torchdsp::async_block_interface;

    py::class_<async_block_interface,
               std::shared_ptr<async_block_interface>>(m, "async_block_interface");;
}
