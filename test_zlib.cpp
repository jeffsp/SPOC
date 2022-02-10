#include <cassert>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "zlib.h"

inline void Verify (const char *e, const char *file, const unsigned line)
{
    std::stringstream s;
    s << "verification failed in " << file << ", line " << line << ": " << e;
    throw std::runtime_error (s.str ());
}

#define verify(e) (void)((e) || (Verify (#e, __FILE__, __LINE__), 0))

std::string zlib_error_string (int ret)
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

std::vector<uint8_t> compress (const std::vector<uint8_t> &input, const int level)
{
    std::vector<uint8_t> output;
    constexpr size_t BUFFER_SIZE = 16384;
    std::vector<uint8_t> input_buffer (BUFFER_SIZE);
    std::vector<uint8_t> output_buffer (BUFFER_SIZE);

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    const auto ret = deflateInit (&strm, level);
    if (ret != Z_OK)
        throw std::runtime_error (zlib_error_string (ret));

    // Index into input buffer
    size_t input_index = 0;

    // How many bytes left to compress?
    size_t bytes_left;

    do {
        // Check logic
        assert (input_index <= input.size ());

        // How many bytes left to compress?
        bytes_left = input.size () - input_index;
        const size_t buffer_size = bytes_left < BUFFER_SIZE ? bytes_left : BUFFER_SIZE;
        std::copy (&input[input_index], &input[input_index + buffer_size], &input_buffer[0]);
        strm.avail_in = buffer_size;
        strm.next_in = &input_buffer[input_index];

        // Should we flush when we are done?
        const int flush = bytes_left <= BUFFER_SIZE ? Z_FINISH : Z_NO_FLUSH;

        do {
            // Setup output buffer
            strm.avail_out = output_buffer.size ();
            strm.next_out = &output_buffer[0];

            // Compress
            const auto ret = deflate (&strm, flush);

            // Check logic
            assert(ret != Z_STREAM_ERROR);

            // Save compressed bytes
            const size_t output_bytes = output_buffer.size () - strm.avail_out;
            output.insert (output.end (), output_buffer.begin (), output_buffer.begin () + output_bytes);
        } while (strm.avail_out == 0);

        assert(strm.avail_in == 0);

        // Go to next position in input buffer
        input_index += buffer_size;

    } while (bytes_left != 0);

    // Close stream
    deflateEnd (&strm);

    return output;
}

std::vector<uint8_t> decompress (const std::vector<uint8_t> &x)
{
    auto y = x;
    return y;
}

/*
int inf(FILE *source, FILE *dest)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[BUFFER_SIZE];
    unsigned char out[BUFFER_SIZE];

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    do {
        strm.avail_in = fread(in, 1, BUFFER_SIZE, source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        do {
            strm.avail_out = BUFFER_SIZE;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = BUFFER_SIZE - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

    } while (ret != Z_STREAM_END);

    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
*/

int main (int argc, char **argv)
{
    using namespace std;

    try
    {
        const auto levels = {
            Z_NO_COMPRESSION,
            Z_BEST_SPEED,
            Z_BEST_COMPRESSION,
            Z_DEFAULT_COMPRESSION};

        // Create datasets
        vector<vector<uint8_t>> datasets;

        // Random bytes
        const size_t N = 10000;
        clog << "Generating " << N << " random bytes " << endl;
        vector<uint8_t> x (N);
        default_random_engine g;
        uniform_int_distribution<int> b (0, 255);
        for (auto &y : x)
            y = b (g);

        // Save it
        datasets.push_back (x);

        // This executable
        clog << "Reading " << argv[0] << endl;
        ifstream ifs (argv[0]);
        if (!ifs)
            throw runtime_error ("Could not open file for reading");
        auto ss = ostringstream {};
        ss << ifs.rdbuf();
        datasets.push_back (vector<uint8_t> (ss.str ().begin (), ss.str ().end ()));

        // Compress them all at all compression levels
        for (const auto &x : datasets)
        {
            for (auto l : levels)
            {
                clog << "Compressing" << endl;
                const auto y = compress (x, l);
                clog << "Decompressing" << endl;
                const auto z = decompress (y);
                //verify (x == z);
                clog << "Original size " << x.size () << endl;
                clog << "Compressed size " << y.size () << endl;
                clog << "Ratio " << x.size () * 100.0 / y.size () << "%" << endl;
            }
        }

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
