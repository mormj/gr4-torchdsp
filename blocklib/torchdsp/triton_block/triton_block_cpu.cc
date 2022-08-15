/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "triton_block_cpu.h"
#include "triton_block_cpu_gen.h"

namespace gr {
namespace torchdsp {

triton_block_cpu::triton_block_cpu(block_args args) : INHERITED_CONSTRUCTORS {}

work_return_t triton_block_cpu::work(work_io& wio)
{
    // Do <+signal processing+>
    // Block specific code goes here
    return work_return_t::OK;
}


} // namespace torchdsp
} // namespace gr