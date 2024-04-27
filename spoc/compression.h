#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <zlib.h>

namespace spoc
{

namespace compression
{

// GCOV_EXCL_START
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
// GCOV_EXCL_STOP

struct zlib_deflator
{
    z_stream s;
    explicit zlib_deflator (const int level)
    {
        s.zalloc = Z_NULL;
        s.zfree = Z_NULL;
        s.opaque = Z_NULL;
        s.avail_in = 0;
        s.next_in = Z_NULL;
        const auto ret = deflateInit (&s, level);
        // GCOV_EXCL_START
        if (ret != Z_OK)
            throw std::runtime_error (zlib_error_string (ret));
        // GCOV_EXCL_STOP
    }
    ~zlib_deflator ()
    {
        deflateEnd (&s);
    }
};

struct zlib_inflator
{
    z_stream s;
    zlib_inflator ()
    {
        s.zalloc = Z_NULL;
        s.zfree = Z_NULL;
        s.opaque = Z_NULL;
        s.avail_in = 0;
        s.next_in = Z_NULL;
        const auto ret = inflateInit (&s);
        // GCOV_EXCL_START
        if (ret != Z_OK)
            throw std::runtime_error (zlib_error_string (ret));
        // GCOV_EXCL_STOP
    }
    ~zlib_inflator ()
    {
        inflateEnd (&s);
    }
};

// Compress 'nbytes' pointed to by 'p'. The 'level' parameter can be 1
// to 9, where 1 is the fastest, and 9 is the best compression. 0 means
// no compression, and -1 means use the default compression.
inline std::vector<uint8_t> compress (const uint8_t *p, const size_t nbytes, const int level = -1)
{
    constexpr size_t BUFFER_SIZE = (1 << 20);
    std::vector<uint8_t> output_buffer (BUFFER_SIZE);
    std::vector<uint8_t> output;

    zlib_deflator deflator (level);

    // Get bytes directly from the input vector
    deflator.s.avail_in = nbytes;
    deflator.s.next_in = const_cast<unsigned char *> (p);

    do {
        // Compress from input to output buffer
        deflator.s.avail_out = BUFFER_SIZE;
        deflator.s.next_out = &output_buffer[0];
        deflate (&deflator.s, Z_FINISH);
        const auto compressed_bytes = BUFFER_SIZE - deflator.s.avail_out;

        // Save compressed bytes
        assert (compressed_bytes <= output_buffer.size ());
        output.insert (output.end (),
            output_buffer.begin (),
            output_buffer.begin () + compressed_bytes);
    } while (deflator.s.avail_out == 0);

    return output;
}

inline std::vector<uint8_t> compress (const std::vector<uint8_t> &input, const int level = -1)
{
    return compress (&input[0], input.size (), level);
}

inline void compress (std::istream &is, std::ostream &os, const int level)
{
    constexpr size_t BUFFER_SIZE = (1 << 20);
    std::vector<uint8_t> input_buffer (BUFFER_SIZE);
    std::vector<uint8_t> output_buffer (BUFFER_SIZE);

    zlib_deflator deflator (level);

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
            // GCOV_EXCL_START
            if (os.fail ())
                throw std::runtime_error (zlib_error_string (Z_ERRNO));
            // GCOV_EXCL_STOP
        } while (deflator.s.avail_out == 0);
    } while (!is.eof ());
}

inline std::vector<uint8_t> decompress (const std::vector<uint8_t> &input)
{
    constexpr size_t BUFFER_SIZE = (1 << 20);
    std::vector<uint8_t> output_buffer (BUFFER_SIZE);
    std::vector<uint8_t> output;

    zlib_inflator inflator;

    // Get bytes directory from input vector
    inflator.s.avail_in = input.size ();
    inflator.s.next_in = const_cast<unsigned char *> (&input[0]);

    do {
        inflator.s.avail_out = BUFFER_SIZE;
        inflator.s.next_out = &output_buffer[0];
        const auto ret = inflate (&inflator.s, Z_NO_FLUSH);
        switch (ret)
        {
            // GCOV_EXCL_START
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                throw std::runtime_error (zlib_error_string (ret));
            // GCOV_EXCL_STOP
        }
        const auto decompressed_bytes = BUFFER_SIZE - inflator.s.avail_out;

        // Save decompressed bytes
        assert (decompressed_bytes <= output_buffer.size ());
        output.insert (output.end (),
            output_buffer.begin (),
            output_buffer.begin () + decompressed_bytes);
    } while (inflator.s.avail_out == 0);

    return output;
}

inline void decompress (const std::vector<uint8_t> &input, uint8_t *output, const size_t output_bytes)
{
    const auto d = decompress (input);
    // GCOV_EXCL_START
    if (d.size () != output_bytes)
        throw std::runtime_error ("Can't decompress: unexpected number of decompressed bytes");;
    // GCOV_EXCL_STOP
    std::copy (&d[0], &d[0] + output_bytes, output);
}

inline void decompress (std::istream &is, std::ostream &os)
{
    constexpr size_t BUFFER_SIZE = (1 << 20);
    std::vector<uint8_t> input_buffer (BUFFER_SIZE);
    std::vector<uint8_t> output_buffer (BUFFER_SIZE);

    zlib_inflator inflator;

    bool done = false;
    do {
        // Read into buffer from stream
        is.read (reinterpret_cast<char *> (&input_buffer[0]), BUFFER_SIZE);
        inflator.s.avail_in = is.gcount();

        // GCOV_EXCL_START
        if (inflator.s.avail_in == 0)
            break;
        // GCOV_EXCL_STOP

        inflator.s.next_in = &input_buffer[0];

        do {
            inflator.s.avail_out = BUFFER_SIZE;
            inflator.s.next_out = &output_buffer[0];
            const auto ret = inflate (&inflator.s, Z_NO_FLUSH);
            // GCOV_EXCL_START
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
            // GCOV_EXCL_STOP
            const auto decompressed_bytes = BUFFER_SIZE - inflator.s.avail_out;

            // Write decompressed bytes
            os.write (reinterpret_cast<char *> (&output_buffer[0]), decompressed_bytes);
            // GCOV_EXCL_START
            if (os.fail ())
                throw std::runtime_error (zlib_error_string (Z_ERRNO));
            // GCOV_EXCL_STOP
        } while (inflator.s.avail_out == 0);
    } while (!done);
}

} // namespace compression

} // namespace spoc
