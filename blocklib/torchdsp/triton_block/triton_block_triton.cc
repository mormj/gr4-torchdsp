/* -*- c++ -*- */
/*
 * Copyright 2022 Block Author
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "triton_block_triton.h"
#include "triton_block_triton_gen.h"

#include <gnuradio/torchdsp/buffer_triton.h>

namespace gr {
namespace torchdsp {

triton_block_triton::triton_block_triton(block_args args) : INHERITED_CONSTRUCTORS
{
    model_ = triton_model::make(args.model_name, args.max_batch_size, args.triton_url);
    if (model_ == nullptr)
        throw std::runtime_error("Could not instantiate triton_model");
    set_output_multiple(model_.get()->get_output_sizes()[0] /
                        model_.get()->get_output_signature()[0]);
    std::cout << "Instantiated block" << std::endl;
}

work_return_t triton_block_triton::work(work_io& wio)
{
    // std::cout << "Got " << noutput_items << " to produce." << std::endl;

    auto noutput_items = wio.outputs()[0].n_items;

    auto in1 = wio.inputs()[0].items<float>();
    auto in2 = wio.inputs()[1].items<float>();
    auto out = wio.outputs()[0].items<float>();

    std::vector<buffer_triton_reader*> in_bufs;
    for (auto& w : wio.inputs()) {
        auto p_buf = w.bufp();
        in_bufs.push_back(static_cast<buffer_triton_reader*>(p_buf));
    }


    std::vector<buffer_triton*> out_bufs;
    for (auto& w : wio.outputs()) {
        auto p_buf = w.bufp();
        out_bufs.push_back(static_cast<buffer_triton*>(p_buf));
    }

    // num_items_per_patch is fixed.
    auto num_items_per_batch =
        model_.get()->get_output_sizes()[0] / model_.get()->get_output_signature()[0];
    auto batch_size = noutput_items / num_items_per_batch;

    model_->infer_batch_zerocopy(in_bufs, out_bufs, batch_size);

    // for (int i=0; i<10; i++)
    // {
    //     std::cout << ((float *)out_bufs[0]->write_ptr())[i] << std::endl;
        
    // }


    wio.produce_each(noutput_items);
    return work_return_t::OK;
}


} // namespace torchdsp
} // namespace gr
