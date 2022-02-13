#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <zlib.h>

namespace spc
{

inline std::string zlib_error_string (int ret)
{
    switch (ret)
    {
        default:
        case Z_ERRNO:
            return std::string ("ZLIB error");
        case Z_STREAM_ERROR:
            return std::string ("Invalid compression level");
        case Z_DATA_ERROR:
            return std::string ("Invalid or incomplete deflate data");
        case Z_MEM_ERROR:
            return std::string ("Out of memory");
        case Z_VERSION_ERROR:
            return std::string ("Zlib version mismatch");
    }
}

struct Deflator
{
    z_stream s;
    Deflator (const int level)
    {
        s.zalloc = Z_NULL;
        s.zfree = Z_NULL;
        s.opaque = Z_NULL;
        s.avail_in = 0;
        s.next_in = Z_NULL;
        const auto ret = deflateInit (&s, level);
        if (ret != Z_OK)
            throw std::runtime_error (zlib_error_string (ret));
    }
    ~Deflator ()
    {
        deflateEnd (&s);
    }
};

struct Inflator
{
    z_stream s;
    Inflator ()
    {
        s.zalloc = Z_NULL;
        s.zfree = Z_NULL;
        s.opaque = Z_NULL;
        s.avail_in = 0;
        s.next_in = Z_NULL;
        const auto ret = inflateInit (&s);
        if (ret != Z_OK)
            throw std::runtime_error (zlib_error_string (ret));
    }
    ~Inflator ()
    {
        inflateEnd (&s);
    }
};

inline void compress (std::istream &is, std::ostream &os, const int level)
{
    constexpr size_t BUFFER_SIZE = (1 << 20);
    std::vector<uint8_t> input_buffer (BUFFER_SIZE);
    std::vector<uint8_t> output_buffer (BUFFER_SIZE);

    Deflator deflator (level);

    do {
        // Read into buffer from stream
        is.read (reinterpret_cast<char *> (&input_buffer[0]), BUFFER_SIZE);
        deflator.s.avail_in = is.gcount();
        deflator.s.next_in = &input_buffer[0];

        do {
            // Compress from input buffer to output buffer
            deflator.s.avail_out = BUFFER_SIZE;
            deflator.s.next_out = &output_buffer[0];
            deflate (&deflator.s, is.eof() ? Z_FINISH : Z_NO_FLUSH);
            const auto compressed_bytes = BUFFER_SIZE - deflator.s.avail_out;

            // Write compressed bytes
            os.write (reinterpret_cast<char *> (&output_buffer[0]), compressed_bytes);
            if (os.fail ())
                throw std::runtime_error (zlib_error_string (Z_ERRNO));
        } while (deflator.s.avail_out == 0);
    } while (!is.eof ());
}

inline void decompress (std::istream &is, std::ostream &os)
{
    constexpr size_t BUFFER_SIZE = (1 << 20);
    std::vector<uint8_t> input_buffer (BUFFER_SIZE);
    std::vector<uint8_t> output_buffer (BUFFER_SIZE);

    Inflator inflator;

    bool done = false;
    do {
        // Read into buffer from stream
        is.read (reinterpret_cast<char *> (&input_buffer[0]), BUFFER_SIZE);
        inflator.s.avail_in = is.gcount();

        if (inflator.s.avail_in == 0)
            break;

        inflator.s.next_in = &input_buffer[0];

        do {
            inflator.s.avail_out = BUFFER_SIZE;
            inflator.s.next_out = &output_buffer[0];
            const auto ret = inflate (&inflator.s, Z_NO_FLUSH);
            switch (ret)
            {
                case Z_NEED_DICT:
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    throw std::runtime_error (zlib_error_string (ret));
                case Z_STREAM_END:
                    done = true;
                break;
            }
            const auto decompressed_bytes = BUFFER_SIZE - inflator.s.avail_out;

            // Write decompressed bytes
            os.write (reinterpret_cast<char *> (&output_buffer[0]), decompressed_bytes);
            if (os.fail ())
                throw std::runtime_error (zlib_error_string (Z_ERRNO));
        } while (inflator.s.avail_out == 0);
    } while (!done);
}

} // namespace spc
