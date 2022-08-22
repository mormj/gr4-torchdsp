#pragma once

#include <http_client.h>

namespace gr {
namespace torchdsp
{
    class async_block_interface
    {
    public:
        virtual void
            set_async_callback(std::function<void(triton::client::InferResult*)>) = 0;
        virtual ~async_block_interface() {}
    };
}
} // namespace gr
