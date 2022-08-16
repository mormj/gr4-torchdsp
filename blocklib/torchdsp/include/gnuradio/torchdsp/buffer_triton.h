#pragma once

#include <string.h>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include <gnuradio/buffer_sm.h>

#include <http_client.h>

namespace tc = triton::client;

namespace gr {
enum class buffer_triton_type { D2D, H2D, D2H, UNKNOWN };

class buffer_triton : public buffer_sm
{
private:
    void* _shared_memory;
    size_t _buffer_size;
    buffer_triton_type _type = buffer_triton_type::UNKNOWN;
    std::string _shm_key;
    std::unique_ptr<tc::InferenceServerHttpClient> _client;

public:
    using sptr = std::shared_ptr<buffer_triton>;
    buffer_triton(size_t num_items,
                  size_t item_size,
                  const std::string& triton_url,
                  buffer_triton_type type,
                  std::shared_ptr<buffer_properties> buf_properties);
    ~buffer_triton();

    static buffer_uptr make(size_t num_items,
                            size_t item_size,
                            std::shared_ptr<buffer_properties> buffer_properties);

    buffer_triton_type type() { return _type; }

    virtual buffer_reader_uptr add_reader(std::shared_ptr<buffer_properties> buf_props,
                                          size_t itemsize);


    virtual bool output_blocked_callback(bool force = false) override
    {
        return output_blocked_callback_logic(force, std::memmove);
    }

    std::string& shm_key() { return _shm_key; }
};

class buffer_triton_reader : public buffer_sm_reader
{
private:
    // logger_ptr d_logger;
    // logger_ptr d_debug_logger;

    buffer_triton* _buffer_triton;

public:
    std::string& shm_key() { return _buffer_triton->shm_key(); }
    buffer_triton_reader(buffer_triton* buffer,
                         std::shared_ptr<buffer_properties> buf_props,
                         size_t itemsize,
                         size_t read_index)
        : buffer_sm_reader(buffer, itemsize, buf_props, read_index)
    {
        _buffer_triton = buffer;
        // gr::configure_default_loggers(d_logger, d_debug_logger, "buffer_triton");
    }

    // virtual void post_read(int num_items);

    virtual bool input_blocked_callback(size_t items_required) override
    {
        // Only singly mapped buffers need to do anything with this callback
        // std::scoped_lock guard(*(_buffer->mutex()));
        std::lock_guard<std::mutex> guard(*(_buffer->mutex()));

        auto items_avail = items_available();

        // Maybe adjust read pointers from min read index?
        // This would mean that *all* readers must be > (passed) the write index
        if (items_avail < items_required && _buffer->write_index() < read_index()) {
            // GR_LOG_DEBUG(d_debug_logger, "Calling adjust_buffer_data ");
            return _buffer_sm->adjust_buffer_data(std::memcpy, std::memmove);
        }

        return false;
    }
};

class buffer_triton_properties : public buffer_properties
{
public:
    // using std::shared_ptr<buffer_properties> = sptr;
    buffer_triton_properties(const std::string& triton_url_, buffer_triton_type buffer_type_)
        : buffer_properties(), _triton_url(triton_url_), _buffer_type(buffer_type_)
    {
        _bff = buffer_triton::make;
    }
    std::string& triton_url() { return _triton_url; }
    buffer_triton_type buffer_type() { return _buffer_type; }
    static std::shared_ptr<buffer_properties>
    make(const std::string& triton_url_ = "localhost:8000",
         buffer_triton_type buffer_type_ = buffer_triton_type::D2D)
    {
        return std::static_pointer_cast<buffer_properties>(
            std::make_shared<buffer_triton_properties>(triton_url_, buffer_type_));
    }

private:
    std::string _triton_url;
    buffer_triton_type _buffer_type;
};


} // namespace gr

#define TRITON_BUFFER_ARGS_H2D buffer_triton_properties::make(buffer_triton_type::H2D)
#define TRITON_BUFFER_ARGS_D2H buffer_triton_properties::make(buffer_triton_type::D2H)
#define TRITON_BUFFER_ARGS_D2D buffer_triton_properties::make(buffer_triton_type::D2D)
