#include <gnuradio/prefs.h>
#include <gnuradio/schedulers/triton/scheduler_triton.h>
#include <yaml-cpp/yaml.h>
namespace gr {
namespace schedulers {

void scheduler_triton::push_message(scheduler_message_sptr msg)
{
    _thread->push_message(msg);
}

void scheduler_triton::initialize(flat_graph_sptr fg, runtime_monitor_sptr fgmon)
{

    auto bufman = std::make_shared<buffer_manager>(_opts.default_buffer_size);
    bufman->initialize_buffers(fg, _default_buf_properties, base());

    auto blocks = fg->calc_used_blocks();
    for (auto& b : blocks) {
        b->populate_work_io();
    }

    _thread = thread_wrapper::make(
        id(), block_group_properties({ blocks }), bufman, fgmon, _opts);

    for (auto& b : blocks) {
        b->set_parent_intf(_thread);
        for (auto& p : b->all_ports()) {
            p->set_parent_intf(_thread); // give a shared pointer to the scheduler class
        }
    }
}

void scheduler_triton::start() { _thread->start(); }
void scheduler_triton::stop() { _thread->stop(); }
void scheduler_triton::wait() { _thread->wait(); }
void scheduler_triton::run()
{
    _thread->start();
    _thread->wait();
}

void scheduler_triton::kill() { _thread->kill(); }

} // namespace schedulers
} // namespace gr


// External plugin interface for instantiating out of tree schedulers
// TODO: name, version, other info methods
extern "C" {
std::shared_ptr<gr::scheduler> factory(const std::string& options)
{
    // const std::string& name = "triton";
    // size_t buf_size = 32768;

    auto opt_yaml = YAML::Load(options);
    gr::schedulers::scheduler_triton_options opts = {};

    opts.default_buffer_size = opt_yaml["buffer_size"].as<size_t>(
        gr::prefs::get_long("scheduler.triton", "default_buffer_size", 32768));
    opts.name = opt_yaml["name"].as<std::string>("triton");
    opts.flush = opt_yaml["flush"].as<bool>(
        gr::prefs::get_bool("scheduler.triton", "flush", true));
    opts.flush_count = opt_yaml["flush_count"].as<long>(
        gr::prefs::get_long("scheduler.triton", "flush_count", 8));
    opts.flush_sleep_ms = opt_yaml["flush_count"].as<long>(
        gr::prefs::get_long("scheduler.triton", "flush_count", 8));

    return gr::schedulers::scheduler_triton::make(opts);
}
}
