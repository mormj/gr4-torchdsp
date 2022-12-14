#pragma once

#include <string>

namespace gr {
namespace schedulers {

struct scheduler_triton_options {
    std::string name = "triton";
    size_t default_buffer_size = 32768;
    bool flush = true;
    size_t flush_count = 8;
    size_t flush_sleep_ms = 10;
};

} // namespace schedulers
} // namespace gr
