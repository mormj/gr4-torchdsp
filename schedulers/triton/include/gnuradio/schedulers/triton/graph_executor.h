#pragma once

#include <gnuradio/block.h>
#include <gnuradio/buffer.h>
#include <gnuradio/buffer_management.h>
#include <gnuradio/executor.h>

#include <map>

namespace gr {
namespace schedulers {

/**
 * @brief Responsible for the execution of a graph
 *
 * Maintains the list of blocks for a particular thread and executes the workflow when
 * run_one_iteration is called
 *
 */
class graph_executor : public executor
{
private:
    std::vector<block_sptr> d_blocks;

    // Move to buffer management
    const int s_fixed_buf_size;
    static const int s_min_items_to_process = 1;
    const size_t s_min_buf_items = 1;

    buffer_manager::sptr _bufman;

    void _sort(std::vector<block_sptr>& blks)
    {
       
        std::sort(blks.begin(), blks.end(), [](block_sptr a, block_sptr b) {
            return a->id() < b->id();
        });

        for (auto& b : blks)
        {
            std::cout << b->id() << std::endl;
        }
    }

public:
    graph_executor(const std::string& name) : executor(name), s_fixed_buf_size(32768){};
    ~graph_executor(){};


    void initialize(buffer_manager::sptr bufman, std::vector<block_sptr>& blocks)
    {
        _bufman = bufman;
        _sort(blocks);
    }

    executor_iteration_status_t
    run_one_iteration(std::vector<block_sptr> blocks = std::vector<block_sptr>());
};

} // namespace schedulers
} // namespace gr