#include "graph_executor.h"

namespace gr {
namespace schedulers {

inline static unsigned int round_down(unsigned int n, unsigned int multiple)
{
    return (n / multiple) * multiple;
}

executor_iteration_status_t
graph_executor::run_one_iteration(std::vector<block_sptr> blocks)
{
    executor_iteration_status_t status;

    // If no blocks are specified for the iteration, then run over all the blocks
    // in the default ordering
    if (blocks.empty()) {
        blocks = d_blocks;
    }

    auto first_block = blocks[0];
    auto last_block = blocks[blocks.size() - 1];
    bool ready = true;

    { // INPUTS
        work_io& wio = first_block->get_work_io();
        wio.reset();

        for (auto& w : wio.inputs()) {
            auto p_buf = w.bufp();
            if (p_buf) {
                auto max_read = p_buf->max_buffer_read();
                auto min_read = p_buf->min_buffer_read();

                buffer_info_t read_info;
                ready = p_buf->read_info(read_info);
                d_debug_logger->debug("read_info {} - {} - {}, total: {}",
                                      first_block->alias(),
                                      read_info.n_items,
                                      read_info.item_size,
                                      read_info.total_items);

                if (!ready)
                    break;

                if (read_info.n_items < s_min_items_to_process ||
                    (min_read > 0 && read_info.n_items < (int)min_read)) {

                    ready = false;
                    break;
                }

                if (max_read > 0 && read_info.n_items > (int)max_read) {
                    read_info.n_items = max_read;
                }


                auto tags = p_buf->get_tags(read_info.n_items);
                w.n_items = read_info.n_items;
            }
        }

        if (!ready) {
            status = executor_iteration_status_t::BLKD_IN;

            // If we are blocked on the input, run the input blocked callback on all
            // inputs of all blocks
            for (auto const& b : blocks) {
                work_io& wio = first_block->get_work_io();
                for (auto& w : wio.inputs()) {
                    auto p_buf = w.bufp();
                    if (p_buf->input_blocked_callback(s_min_items_to_process) &&
                        b == first_block) {
                        w.port->notify_scheduler_action(
                            scheduler_action_t::NOTIFY_OUTPUT);
                    }
                }
            }

            return status;
        }
    }

    { // OUTPUTS
        work_io& wio = last_block->get_work_io();
        wio.reset();
        // for each output port of the block
        for (auto& w : wio.outputs()) {

            // When a block has multiple output buffers, it adds the restriction
            // that the work call can only produce the minimum available across
            // the buffers.

            size_t max_output_buffer = std::numeric_limits<int>::max();


            auto p_buf = w.bufp();
            if (p_buf) {
                auto max_fill = p_buf->max_buffer_fill();
                auto min_fill = p_buf->min_buffer_fill();

                buffer_info_t write_info;
                ready = p_buf->write_info(write_info);
                d_debug_logger->debug("write_info {} - {} @ {} {}, total: {}",
                                      last_block->alias(),
                                      write_info.n_items,
                                      write_info.ptr,
                                      write_info.item_size,
                                      write_info.total_items);

                size_t tmp_buf_size = write_info.n_items;
                if (tmp_buf_size < s_min_buf_items ||
                    (min_fill > 0 && tmp_buf_size < min_fill)) {
                    ready = false;

                    break;
                }

                if (tmp_buf_size < max_output_buffer)
                    max_output_buffer = tmp_buf_size;

                if (max_fill > 0 && max_output_buffer > max_fill) {
                    max_output_buffer = max_fill;
                }

                if (last_block->output_multiple_set()) {
                    d_debug_logger->debug("output_multiple {}", last_block->output_multiple());
                    max_output_buffer =
                        round_down(max_output_buffer, last_block->output_multiple());
                }

                if (max_output_buffer <= 0) {
                    ready = false;
                }

                if (!ready)
                    break;


                std::vector<tag_t> tags; // needs to be associated with edge buffers

                w.n_items = max_output_buffer;
            }
        }

        if (!ready) {
            status = executor_iteration_status_t::BLKD_OUT;
            for (auto const& b : blocks) {
                work_io& wio = b->get_work_io();
                for (auto& w : wio.outputs()) {
                    auto p_buf = w.bufp();
                    if (p_buf->output_blocked_callback(false) && b == last_block) {
                        w.port->notify_scheduler_action(scheduler_action_t::NOTIFY_INPUT);
                    }
                }
            }

            return status;
        }
    }

    for (auto const& b : blocks) {
        if (b->is_hier()) {
            continue;
        }

        work_io& wio = b->get_work_io();
        wio.reset();


        if (ready) {
            work_return_t ret;
            while (true) {

                if (!wio.outputs().empty()) {
                    d_debug_logger->debug("do_work (output) for {}, {}",
                                          b->alias(),
                                          wio.outputs()[0].n_items);
                } else if (!wio.inputs().empty()) {
                    d_debug_logger->debug("do_work (input) for {}, {}",
                                          b->alias(),
                                          wio.inputs()[0].n_items);
                } else {
                    d_debug_logger->debug("do_work for {}", b->alias());
                }


                ret = b->do_work(wio);
                d_debug_logger->debug("do_work returned {}", (int)ret);
                // ret = work_return_t::OK;

                if (ret == work_return_t::DONE) {
                    status = executor_iteration_status_t::DONE;
                    d_debug_logger->debug("pbs[{}]: {}", b->id(), (int)status);
                    break;
                } else if (ret == work_return_t::OK) {
                    status = executor_iteration_status_t::READY;
                    d_debug_logger->debug("pbs[{}]: {}", b->id(), (int)status);

                    // If a source block, and no outputs were produced, mark as BLKD_IN
                    if (wio.inputs().empty() && !wio.outputs().empty()) {
                        size_t max_output = 0;
                        for (auto& w : wio.outputs()) {
                            max_output = std::max(w.n_produced, max_output);
                        }
                        if (max_output <= 0) {
                            status = executor_iteration_status_t::BLKD_IN;
                            d_debug_logger->debug("pbs[{}]: {}", b->id(), (int)status);
                        }
                    }


                    break;
                }
            }
        }
    }


    return status;
}

} // namespace schedulers
} // namespace gr
