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

triton_block_cpu::triton_block_cpu(block_args args) : INHERITED_CONSTRUCTORS
{
    model_ = triton_model::make(args.model_name, args.max_batch_size, args.triton_url);
    if (model_ == nullptr)
        throw std::runtime_error("Could not instantiate triton_model");
    set_output_multiple(model_.get()->get_output_sizes()[0] /
                        model_.get()->get_output_signature()[0]);
    std::cout << "Instantiated block" << std::endl;
}

work_return_t triton_block_cpu::work(work_io& wio)
{
    // std::cout << "Got " << noutput_items << " to produce." << std::endl;

    auto noutput_items = wio.outputs()[0].n_items;

    std::vector<const char*> in_ptrs;
    for (const auto& item : wio.inputs())
        in_ptrs.push_back(item.items<char>());


    std::vector<char*> out_ptrs;
    for (const auto& item : wio.outputs())
        out_ptrs.push_back(item.items<char>());

    // num_items_per_patch is fixed.
    auto num_items_per_batch =
        model_.get()->get_output_sizes()[0] / model_.get()->get_output_signature()[0];
    auto batch_size = noutput_items / num_items_per_batch;
    model_->infer_batch(in_ptrs, out_ptrs, batch_size);

    wio.produce_each(noutput_items);
    return work_return_t::OK;
}


} // namespace torchdsp
} // namespace gr