/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/torchdsp/add.h>

namespace gr {
namespace torchdsp {

template <class T>
class add_cpu : public add<T>
{
public:
    add_cpu(const typename add<T>::block_args& args);

    work_return_t work(work_io&) override;

private:
    // Declare private variables here
};


} // namespace torchdsp
} // namespace gr
