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

    void set_async_callback(std::function<void(triton::client::InferResult*)> cb) override
    {
        _callback = cb;
    }

private:
    std::unique_ptr<triton_model> model_;

    // Initialize with a callback that does nothing
    std::function<void(triton::client::InferResult*)> _callback =
        [](triton::client::InferResult* r) {};
};

} // namespace torchdsp
} // namespace gr