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
#include <gnuradio/torchdsp/triton_model.h>

namespace gr {
namespace torchdsp {

class triton_block_triton : public virtual triton_block
{
public:
    triton_block_triton(block_args args);
    work_return_t work(work_io& wio) override;

private:
    std::unique_ptr<triton_model> model_;
};

} // namespace torchdsp
} // namespace gr