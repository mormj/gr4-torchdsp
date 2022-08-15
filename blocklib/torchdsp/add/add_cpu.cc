/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "add_cpu.h"
#include "add_cpu_gen.h"

namespace gr {
namespace torchdsp {

template <class T>
add_cpu<T>::add_cpu(const typename add<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T)
{
}

template <class T>
work_return_t add_cpu<T>::work(work_io&)
{
    // Do work specific code here
    return work_return_t::OK;
}

} /* namespace torchdsp */
} /* namespace gr */
