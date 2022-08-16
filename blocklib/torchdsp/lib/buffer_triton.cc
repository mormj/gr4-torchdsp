#include <string.h>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include <gnuradio/nodeid_generator.h>

#include "shm_utils.h"
#include <gnuradio/torchdsp/buffer_triton.h>
#include <http_client.h>


static bool is_server_healthy(
    const std::unique_ptr<tc::InferenceServerHttpClient>& client) {
    bool is_live;
    tc::Error err = client->IsServerLive(&is_live);
    if (!(err.IsOk() && is_live))
        std::cerr << "Could not connect to Triton Inference Server -- Server not live"
                  << std::endl;

    std::cerr << err.Message() << std::endl;

    bool is_ready;
    err = client->IsServerReady(&is_ready);
    if (!(err.IsOk() && is_ready))
        std::cerr << "Could not connect to Triton Inference Server -- Server not ready"
                  << std::endl;

    return is_ready & is_live;
}


namespace gr {
buffer_triton::buffer_triton(size_t num_items,
                             size_t item_size,
                             const std::string& triton_url,
                             buffer_triton_type type,
                             std::shared_ptr<buffer_properties> buf_properties)
    : gr::buffer_sm(num_items, item_size, buf_properties), _type(type)
{

    tc::Error err = tc::InferenceServerHttpClient::Create(&_client, triton_url, false);
    if (!err.IsOk()) {
        throw std::runtime_error("Unable to create Triton Shared Memory Segment");
    }
    if (!is_server_healthy(_client)) {
        std::cerr << "Failed to connect to TIS instance at " << triton_url << std::endl;
        return;
    }

    int shm_fd_ip;
    _shm_key = std::to_string(nodeid_generator::get_id()) + std::string("gr_") + nodeid_generator::get_unique_string();
    _buffer_size = num_items * item_size;

    tc::Error error = tc::CreateSharedMemoryRegion(std::string("/")+_shm_key, _buffer_size, &shm_fd_ip);
    tc::MapSharedMemory(shm_fd_ip, 0, _buffer_size, (void**)&_shared_memory);
    tc::CloseSharedMemory(shm_fd_ip);
    if (!error.IsOk()) {
        throw std::runtime_error("Unable to create Triton Shared Memory Segment");
    }

    std::cout << fmt::format("{}: {}", _shared_memory, _shm_key) << std::endl;

    err =_client->RegisterSystemSharedMemory(
        _shm_key, std::string("/")+_shm_key, _buffer_size);
    std::cout << err << std::endl;
    if (!err.IsOk()) {
        throw std::runtime_error("Unable to create Triton Shared Memory Segment");
    }
    
    set_type("buffer_triton_" + std::to_string((int)_type));
}
buffer_triton::~buffer_triton()
{
    tc::UnmapSharedMemory(_shared_memory, _buffer_size);
    tc::UnlinkSharedMemoryRegion(std::string("/")+_shm_key);
    _client->UnregisterSystemSharedMemory(_shm_key);
}

buffer_uptr buffer_triton::make(size_t num_items,
                                size_t item_size,
                                std::shared_ptr<buffer_properties> buffer_properties)
{
    auto cbp = std::static_pointer_cast<buffer_triton_properties>(buffer_properties);
    if (cbp != nullptr) {
        return buffer_uptr(new buffer_triton(
            num_items, item_size, cbp->triton_url(), cbp->buffer_type(), buffer_properties));
    } else {
        throw std::runtime_error(
            "Failed to cast buffer properties to buffer_triton_properties");
    }
}

buffer_reader_uptr buffer_triton::add_reader(std::shared_ptr<buffer_properties> buf_props,
                                             size_t itemsize)
{
    auto r =
        std::make_unique<buffer_triton_reader>(this, buf_props, itemsize, _write_index);
    _readers.push_back(r.get());
    return r;
}


} // namespace gr
