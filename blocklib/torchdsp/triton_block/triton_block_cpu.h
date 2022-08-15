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

#include <gnuradio/torchdsp/triton_block.h>

namespace gr {
namespace torchdsp {

class triton_block_cpu : public virtual triton_block
{
public:
    triton_block_cpu(block_args args);
    work_return_t work(work_io& wio) override;

private:
    // private variables here
};

} // namespace torchdsp
} // namespace gr