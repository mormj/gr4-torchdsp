/* -*- c++ -*- */
/*
 * Copyright 2022 Peraton Labs, Inc..
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TORCHDSP_TRITON_MODEL_H
#define INCLUDED_TORCHDSP_TRITON_MODEL_H

#include <gnuradio/sync_block.h>
#include <gnuradio/torchdsp/api.h>
#include <gnuradio/torchdsp/buffer_triton.h>

namespace gr {
namespace torchdsp {

/*!
 * \brief <+description of block+>
 * \ingroup torchdsp
 *
 */
class TORCHDSP_API triton_model
{
public:
    typedef std::unique_ptr<triton_model> sptr;

    // Gotta have a virtual destructor to be able to
    // call reset() on a unique_ptr.
    virtual ~triton_model() = default;

    // Interface to GNU Radio Block
    virtual int get_num_inputs() = 0;
    virtual int get_num_outputs() = 0;
    virtual std::vector<int> get_input_sizes() = 0;
    virtual std::vector<int> get_output_sizes() = 0;
    virtual std::vector<int> get_input_signature() = 0;
    virtual std::vector<int> get_output_signature() = 0;
    virtual void
    infer_batch_zerocopy(std::vector<buffer_triton_reader*>& in_buffers,
                         std::vector<buffer_triton*>& out_buffers,
                         size_t batch_size,
                         std::function<void(triton::client::InferResult*)> = nullptr) = 0;
    /*!
     * \brief Return a shared_ptr to a new instance of torchdsp::triton_model.
     *
     * To avoid accidental use of raw pointers, torchdsp::triton_model's
     * constructor is in a private implementation
     * class. torchdsp::triton_model::make is the public interface for
     * creating new instances.
     */
    static sptr make(const std::string& model_name,
                     const bool async,
                     const std::string& triton_url = "localhost:8000");
};

} // namespace torchdsp
} // namespace gr

#endif /* INCLUDED_TORCHDSP_TRITON_MODEL_H */
