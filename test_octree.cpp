#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include "compress.h"
#include "extent.h"
#include "test_utils.h"


namespace spc
{

// Given a reference point in 3D space, in which octant does a point
// lie relative to that reference?
//
// There are 2^3=8 possibilities. If the reference is at location
//
//    0.0, 0.0, 0.0
//
// A second point at x,y,z may be located in one of 8 octants:
//
//    bit value
//    0   x>0
//    1   y>0
//    2   z>0
unsigned get_octant_index (const point<double> &p, const point<double> &ref)
{
    // Get octree node index = 0..7
    const unsigned index =
        ((p.x < ref.x) << 0) |
        ((p.y < ref.y) << 1) |
        ((p.z < ref.z) << 2);
    assert (index < 8);
    return index;
}

struct octree_node
{
    std::array<std::unique_ptr<octree_node>, 8> octants;
    std::unique_ptr<std::vector<point<double>>> deltas;
};

template<typename NF, typename LF>
void dfs (const std::unique_ptr<octree_node> &node,
    NF node_function,
    LF leaf_function)
{
    // Call node function on all nodes
    node_function (node);

    // It's a leaf if it has no children
    bool is_leaf = true;
    for (const auto &n : node->octants)
    {
        // Skip nulls
        if (n == nullptr)
            continue;

        // Can't be a leaf if it has children
        is_leaf = false;

        // Recurse into non-null branches
        dfs (n, node_function, leaf_function);
    }

    // Call leaf if needed
    if (is_leaf)
        leaf_function (node);
}

template<typename T>
uint8_t to_byte (const T &octants)
{
    uint8_t b = 0;
    for (size_t i = 0; i < octants.size (); ++i)
    {
        if (octants[i] == nullptr)
            continue;
        b |= (1 << i);
    }
    return b;
}

void append_double (const double d, std::vector<uint8_t> &x)
{
    const uint8_t *p = reinterpret_cast<const uint8_t *> (&d);
    x.insert (x.end (), p, p + sizeof(double));
}

double bytes_to_double (const uint8_t *p)
{
    const double d = *(reinterpret_cast<const double *> (p));
    return d;
}

std::vector<uint8_t> encode_extent (const spc::extent<double> &e)
{
    std::vector<uint8_t> bytes;
    append_double (e.minp.x, bytes);
    append_double (e.minp.y, bytes);
    append_double (e.minp.z, bytes);
    append_double (e.maxp.x, bytes);
    append_double (e.maxp.y, bytes);
    append_double (e.maxp.z, bytes);
    return bytes;
}

spc::extent<double> decode_extent (const std::vector<uint8_t> &bytes)
{
    spc::extent<double> e;
    e.minp.x = bytes_to_double (&bytes[0] + 0 * sizeof(double));
    e.minp.y = bytes_to_double (&bytes[0] + 1 * sizeof(double));
    e.minp.z = bytes_to_double (&bytes[0] + 2 * sizeof(double));
    e.maxp.x = bytes_to_double (&bytes[0] + 3 * sizeof(double));
    e.maxp.y = bytes_to_double (&bytes[0] + 4 * sizeof(double));
    e.maxp.z = bytes_to_double (&bytes[0] + 5 * sizeof(double));
    return e;
}

std::vector<uint8_t> encode_octree (const std::unique_ptr<octree_node> &root)
{
    std::vector<uint8_t> x;
    const auto node_function = [&] (const std::unique_ptr<octree_node> &node)
    {
        // Encode the octant pointers
        x.push_back (to_byte (node->octants));
    };
    const auto leaf_function = [&] (const std::unique_ptr<octree_node> &node) { };
    spc::dfs (root, node_function, leaf_function);
    return x;
}

std::vector<uint8_t> encode_point_counts (const std::unique_ptr<octree_node> &root)
{
    std::vector<uint64_t> point_counts;
    const auto node_function = [&] (const std::unique_ptr<octree_node> &node) { };
    const auto leaf_function = [&] (const std::unique_ptr<octree_node> &node)
    {
        // Save the point location
        point_counts.push_back (node->deltas->size ());
    };
    spc::dfs (root, node_function, leaf_function);
    // Output bytes
    std::vector<uint8_t> x;
    // Encode the point counts (NOT PORTABLE)
    const uint8_t *pc = reinterpret_cast<const uint8_t *> (&point_counts[0]);
    x.insert (x.end (), pc, pc + point_counts.size () * sizeof(double));
    return x;
}

std::vector<uint8_t> encode_point_deltas (const std::unique_ptr<octree_node> &root)
{
    std::vector<point<double>> deltas;
    const auto node_function = [&] (const std::unique_ptr<octree_node> &node) { };
    const auto leaf_function = [&] (const std::unique_ptr<octree_node> &node)
    {
        // Save the deltas
        deltas.insert (deltas.end (),
            node->deltas->begin (),
            node->deltas->end ());
    };
    spc::dfs (root, node_function, leaf_function);
    // Output bytes
    std::vector<uint8_t> x;
    // Save the point deltas (NOT PORTABLE)
    const uint8_t *pd = reinterpret_cast<const uint8_t *> (&deltas[0]);
    x.insert (x.end (), pd, pd + deltas.size () * 3 * sizeof(double));
    return x;
}

void add_octree_point (const std::unique_ptr<octree_node> &node,
    const point<double> &p,
    const extent<double> &e,
    const unsigned max_depth,
    const unsigned current_depth)
{
    // Check logic
    assert (node != nullptr);
    assert (all_less_equal (e.minp, e.maxp));
    assert (all_less_equal (e.minp, p));
    assert (all_less_equal (p, e.maxp));

    // Terminal case
    if (current_depth == max_depth)
    {
        // Allocate delta container if needed
        if (node->deltas == nullptr)
            node->deltas.reset (new std::vector<point<double>>);

        // Get the octant corner point
        const auto q = e.minp;

        // Check logic
        assert (all_less_equal (q, p));

        // Subtract octant corner from this point
        const auto d = p - q;

        // Save the delta
        node->deltas->push_back (d);
        return;
    }

    // Get midpoint between min and max extent
    const point<double> midp {
        (e.minp.x + e.maxp.x) / 2.0,
        (e.minp.y + e.maxp.y) / 2.0,
        (e.minp.z + e.maxp.z) / 2.0};

    // Which octant does 'p' belong to?
    const unsigned octant_index = get_octant_index (p, midp);

    // Allocate new node if needed
    if (node->octants[octant_index] == nullptr)
        node->octants[octant_index].reset (new octree_node);

    // Get the extent on the new node
    const auto new_e = get_new_extent (e, p, midp);

    add_octree_point (node->octants[octant_index], p, new_e, max_depth, current_depth + 1);
}

void add_octree_octant (const std::unique_ptr<octree_node> &node,
    const extent<double> &e,
    const std::vector<uint8_t> &bytes,
    size_t &byte_index)
{
    // Check logic
    assert (node != nullptr);
    assert (byte_index <= bytes.size ());
    assert (all_less_equal (e.minp, e.maxp));

    // Terminating case
    if (byte_index == bytes.size ())
        return;

    // Get the octant flags
    const uint8_t b = bytes[byte_index];

    // Is it a leaf?
    if (b == 0)
        return;

    // Get midpoint between min and max extent
    const point<double> midp {
        (e.minp.x + e.maxp.x) / 2.0,
        (e.minp.y + e.maxp.y) / 2.0,
        (e.minp.z + e.maxp.z) / 2.0};

    // How many octants are occupied in this extent?
    for (size_t octant_index = 0; octant_index < 8; ++octant_index)
    {
        // Skip zero flags
        if (((b >> octant_index) & 1) == 0)
            continue;

        // Allocate new node
        node->octants[octant_index].reset (new octree_node);

        // Get the extent of the new node
        const auto new_e = get_new_extent (e, octant_index, midp);

        // Recurse into the new octant
        add_octree_octant (node->octants[octant_index], new_e, bytes, ++byte_index);
    }
}

class octree
{
    private:
    std::unique_ptr<octree_node> root;

    public:
    // CTOR
    octree (const size_t max_depth,
        const std::vector<point<double>> &points)
        : root (nullptr)
    {
        // Make sure we have data
        if (points.empty ())
            return;

        // Allocate the root node
        root.reset (new octree_node);

        // Get the extent min/max points
        const auto e = get_extent (points);

        // Add the points
        for (const auto &p : points)
            add_octree_point (root, p, e, max_depth, 0);
    }
    // CTOR
    octree (const std::vector<uint8_t> &bytes,
        const spc::extent<double> &e)
        : root (nullptr)
    {
        // Create the tree
        root.reset (new octree_node);

        // Create the nodes from the encoded bytes
        size_t byte_index = 0;
        add_octree_octant (root, e, bytes, byte_index);
    }
    const std::unique_ptr<octree_node> &get_root () const { return root; }
};

std::vector<uint8_t> encode (std::vector<point<double>> &points,
        const size_t max_depth = 10)
{
    octree o (max_depth, points);
    const auto x1 = encode_octree (o.get_root ());
    const auto x2 = encode_point_counts (o.get_root ());
    const auto x3 = encode_point_deltas (o.get_root ());

    // Output
    std::vector<uint8_t> x;
    x.insert (x.end (), x1.begin (), x1.end ());
    x.insert (x.end (), x2.begin (), x2.end ());
    x.insert (x.end (), x3.begin (), x3.end ());
    return x;
}

std::vector<point<double>> decode (const std::vector<uint8_t> &x)
{
    std::vector<point<double>> y;
    return y;
}

} // namespace spc

void print_info (std::ostream &os, const spc::octree &o)
{
    unsigned nodes = 0;
    unsigned leafs = 0;
    unsigned total_points = 0;
    const auto node_function = [&] (const std::unique_ptr<spc::octree_node> &node)
    {
        ++nodes;
    };
    const auto leaf_function = [&] (const std::unique_ptr<spc::octree_node> &node)
    {
        ++leafs;
        total_points += node->deltas->size ();
    };
    spc::dfs (o.get_root (), node_function, leaf_function);
    os << " nodes " << nodes
        << " leafs " << leafs
        << " total_points " << total_points
        << " points/leaf " << total_points * 1.0 / leafs << std::endl;
}

void test (const size_t N,
    const int min_exponent = std::numeric_limits<double>::min_exponent / 2,
    const int max_exponent = std::numeric_limits<double>::max_exponent / 2)
{
    using namespace std;
    using namespace spc;

    clog << "Generating " << N << " random points" << endl;

    vector<point<double>> points (N);

    // Generate random doubles at varying scales.
    //
    // We don't want a uniform distribution.
    std::default_random_engine g;
    std::uniform_real_distribution<double> d (-1.0, 1.0);
    std::uniform_int_distribution<int> exp (min_exponent, max_exponent);

    for (auto &p : points)
    {
        const double x = std::ldexp (d (g), exp (g));
        const double y = std::ldexp (d (g), exp (g));
        const double z = std::ldexp (d (g), exp (g));
        p = point<double> {x, y, z};
    }

    clog << "Encoding " << N << " random points" << endl;

    const size_t max_depth = 10;
    octree o (max_depth, points);

    print_info (clog, o);

    // Get the extent min/max points
    const auto e = get_extent (points);

    {
    const auto x = encode_extent (e);
    const auto y = decode_extent (x);
    verify (y == e);
    }

    {
    const auto x = encode_octree (o.get_root ());
    octree o2 (x, e);
    const auto y = encode_octree (o2.get_root ());
    verify (x == y);
    }

    {
    const auto x = encode_point_counts (o.get_root ());
    const auto y = spc::compress (x);
    const auto z = spc::decompress (y);
    verify (x == z);
    }
    return;

    {
    const auto x = encode_point_deltas (o.get_root ());
    const auto y = spc::compress (x);
    const auto z = spc::decompress (y);
    verify (x == z);
    }

    {
        // Copy the points
        auto x (points);
        // Encode them
        auto y = encode (x);
        // Get decoded points
        auto z = decode (y);
        // Sort them because order is not preserved in encoded bytes
        sort (begin (x), end (x));
        sort (begin (z), end (z));
        // Check results
        verify (x == z);
    }
}

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spc;

    try
    {
        test (10, 0, 0);
        test (10, 1, 10);
        test (10, -10, 10);
        test (10'000, 0, 0);
        test (10'000, 0, 10);
        test (10'000, -10, 10);
        test (10'000, 0, 20);
        test (10'000, -20, 20);
        test (10'000);
        test (1'000'000);
        test (1'000'000, 0, 0);
        test (1'000'000, -10, 10);
        test (10'000'000);
        test (11'000'000, 0, 20);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
