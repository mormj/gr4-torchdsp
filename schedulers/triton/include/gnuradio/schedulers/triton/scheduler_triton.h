#include <gnuradio/block_group_properties.h>
#include <gnuradio/torchdsp/buffer_triton.h>
// #include <gnuradio/buffer_cpu_simple.h>
#include <gnuradio/graph_utils.h>
#include <gnuradio/scheduler.h>

#include "scheduler_triton_options.h"
#include "thread_wrapper.h"
namespace gr {
namespace schedulers {


/**
 * @brief Triton Scheduler
 * 
 * @details The Triton scheduler is a domain specific scheduler that 
 * relies on the Triton Inference Server async methods to sequence
 * the work calls.
 * 
 * This means that work calls for blocks handed to this scheduler should 
 * return immediately after queuing up the work on the TIS client. 
 * 
 * Restrictions
 * 1) Blocks must consume all their given inputs
 * 2) Must be sync blocks
 * 
 * Could later be made to be more complex and handle general blocks
 * 
 */

class scheduler_triton : public scheduler
{
private:
    thread_wrapper::sptr _thread;
    const scheduler_triton_options _opts;

public:
    using sptr = std::shared_ptr<scheduler_triton>;
    static sptr make(const scheduler_triton_options& opts = {})
    {
        return std::make_shared<scheduler_triton>(opts);
    }
    scheduler_triton(const scheduler_triton_options& opts) : scheduler(opts.name), _opts(opts)
    {
        _default_buf_properties =
            buffer_triton_properties::make();
    }
    ~scheduler_triton() override{};

    void push_message(scheduler_message_sptr msg) override;

    /**
     * @brief Initialize the triton scheduler
     *
     * Creates a single-threaded scheduler for each block group, then for each block that
     * is not part of a block group
     *
     * @param fg subgraph assigned to this triton scheduler
     * @param fgmon sptr to flowgraph monitor object
     * @param block_sched_map for each block in this flowgraph, a map of neighboring
     * schedulers
     */
    void initialize(flat_graph_sptr fg, runtime_monitor_sptr fgmon) override;
    void start() override;
    void stop() override;
    void wait() override;
    void run();
    void kill() override;
};
} // namespace schedulers
} // namespace gr
