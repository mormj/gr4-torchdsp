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

class buffer_triton : public buffer
{
private:
    void* _shared_memory;
    uint8_t* _buffer;
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

    void* read_ptr(size_t index) { return (void*)&_buffer[index]; }
    void* write_ptr() { return (void*)&_buffer[_write_index]; }

    void post_write(int num_items)
    {
        std::scoped_lock guard(_buf_mutex);

        size_t bytes_written = num_items * _item_size;
        size_t wi1 = _write_index;
        size_t wi2 = _write_index + _buf_size;
        // num_items were written to the buffer
        // copy the data to the second half of the buffer

        size_t num_bytes_1 = std::min(_buf_size - wi1, bytes_written);
        size_t num_bytes_2 = bytes_written - num_bytes_1;

        memcpy(&_buffer[wi2], &_buffer[wi1], num_bytes_1);
        if (num_bytes_2)
            memcpy(&_buffer[0], &_buffer[_buf_size], num_bytes_2);


        // advance the write pointer
        _write_index += bytes_written;
        if (_write_index >= _buf_size) {
            _write_index -= _buf_size;
        }

        _total_written += num_items;
    }

    std::string& shm_key() { return _shm_key; }

    size_t space_available() override
    {

        // Find the max number of bytes available across readers
        uint64_t n_available = 0;
        for (auto& r : _readers) {
            auto n = r->bytes_available();
            if (n > n_available) {
                n_available = n;
            }
        }

        int space_in_items = (_num_items * _item_size - n_available) / _item_size;

        if (space_in_items < 0)
            space_in_items = 0;
        space_in_items = std::min(space_in_items,
                                  (int)(_num_items / 2)); // move to a max_fill parameter

        return space_in_items;
    }
};

class buffer_triton_reader : public buffer_reader
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
        : buffer_reader(buffer, buf_props, itemsize, read_index)
    {
        _buffer_triton = buffer;
        // gr::configure_default_loggers(d_logger, d_debug_logger, "buffer_triton");
    }

    void post_read(int num_items)
    {
        std::scoped_lock guard(_rdr_mutex);

        // advance the read pointer
        _read_index += num_items * _itemsize; // _buffer->item_size();
        if (_read_index >= _buffer->buf_size()) {
            _read_index -= _buffer->buf_size();
        }
        _total_read += num_items;
    }

    size_t bytes_available() override
    {
        size_t w = _buffer->write_index();
        size_t r = _read_index;

        if (w < r)
            w += _buffer->buf_size();

        if (w == r && _buffer->total_written() > _total_read )
        {
            return _buffer->buf_size();
        }

        return (w - r);
    }
};

class buffer_triton_properties : public buffer_properties
{
public:
    // using std::shared_ptr<buffer_properties> = sptr;
    buffer_triton_properties(const std::string& triton_url_,
                             buffer_triton_type buffer_type_)
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
