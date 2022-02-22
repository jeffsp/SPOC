#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>
#include "compress.h"
#include "extent.h"
#include "test_utils.h"


namespace spc
{

unsigned get_octant_index (const point<double> &p, const point<double> &midp)
{
    // Get octree node index = 0..7
    const unsigned index =
        ((p.x < midp.x) << 0) |
        ((p.y < midp.y) << 1) |
        ((p.z < midp.z) << 2);
    assert (index < 8);
    return index;
}

struct octree_node;
using octant_ptrs = std::array<octree_node *, 8>;

struct octree_node
{
    // Arrays are not default initialized for standard types
    octant_ptrs octants = { nullptr };
    extent<double> e;
    size_t total_points = 0;
    std::vector<point<double>> points;
};

template<typename NODE_FUNCTION, typename LEAF_FUNCTION>
void dfs (const octree_node *root,
    NODE_FUNCTION node_function,
    LEAF_FUNCTION leaf_function)
{
    // Call node function on all nodes
    node_function (root);

    // It's a leaf if it has no children
    bool is_leaf = true;
    for (auto n : root->octants)
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
        leaf_function (root);
}

uint8_t to_byte (const octant_ptrs &octants)
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

std::vector<uint8_t> encode_octree (const octree_node *root)
{
    std::vector<uint8_t> x;
    const auto node_function = [&] (const octree_node *root)
    {
        x.push_back (to_byte (root->octants));
    };
    const auto leaf_function = [&] (const octree_node *root)
    {
    };
    spc::dfs (root, node_function, leaf_function);
    return x;
}

std::vector<uint8_t> encode_point_counts (const octree_node *root)
{
    std::vector<uint64_t> point_counts;
    const auto node_function = [&] (const octree_node *root) { };
    const auto leaf_function = [&] (const octree_node *root)
    {
        // Encode the point locations
        point_counts.push_back (root->points.size ());
    };
    spc::dfs (root, node_function, leaf_function);
    // Output bytes
    std::vector<uint8_t> x;
    // Save the point counts (NOT PORTABLE)
    const uint8_t *pc = reinterpret_cast<const uint8_t *> (&point_counts[0]);
    x.insert (x.end (), pc, pc + point_counts.size () * sizeof(double));
    return x;
}

std::vector<uint8_t> encode_point_deltas (const octree_node *root)
{
    std::vector<point<double>> point_deltas;
    const auto node_function = [&] (const octree_node *root) { };
    const auto leaf_function = [&] (const octree_node *root)
    {
        // Get min point in octant
        const auto q = root->e.minp;
        for (size_t i = 0; i < root->points.size (); ++i)
        {
            // Get the point
            const auto p = root->points[i];
            assert (all_less_equal (q, p));
            // Subtract octant min from this point
            const auto d = p - q;
            // Save the delta
            point_deltas.push_back (d);
        }
    };
    spc::dfs (root, node_function, leaf_function);
    // Output bytes
    std::vector<uint8_t> x;
    // Save the point deltas (NOT PORTABLE)
    const uint8_t *pd = reinterpret_cast<const uint8_t *> (&point_deltas[0]);
    x.insert (x.end (), pd, pd + point_deltas.size () * 3 * sizeof(double));
    return x;
}

void add_octree_point (octree_node * const root,
    const point<double> &p,
    const unsigned max_depth,
    const unsigned current_depth)
{
    // Check logic
    assert (root != nullptr);
    assert (all_less_equal (root->e.minp, root->e.maxp));
    assert (all_less_equal (root->e.minp, p));
    assert (all_less_equal (p, root->e.maxp));

    // Count the point
    ++root->total_points;

    // Terminal case
    if (current_depth == max_depth)
    {
        root->points.push_back (p);
        return;
    }

    // Get midpoint between min and max extent
    const point<double> midp {
        (root->e.minp.x + root->e.maxp.x) / 2.0,
        (root->e.minp.y + root->e.maxp.y) / 2.0,
        (root->e.minp.z + root->e.maxp.z) / 2.0};

    // Which octant does 'p' belong to?
    const unsigned octant_index = get_octant_index (p, midp);

    // Allocate new node if needed
    if (root->octants[octant_index] == nullptr)
    {
        root->octants[octant_index] = new octree_node;
        // Set the extent on the new node
        root->octants[octant_index]->e = get_new_extent (root->e, p, midp);
    }

    add_octree_point (root->octants[octant_index], p, max_depth, current_depth + 1);
}

void add_octree_octant (octree_node * const root,
    const std::vector<uint8_t> &bytes,
    size_t &byte_index)
{
    // Check logic
    assert (root != nullptr);
    assert (byte_index <= bytes.size ());
    assert (all_less_equal (root->e.minp, root->e.maxp));

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
        (root->e.minp.x + root->e.maxp.x) / 2.0,
        (root->e.minp.y + root->e.maxp.y) / 2.0,
        (root->e.minp.z + root->e.maxp.z) / 2.0};

    // How many octants are occupied in this extent?
    for (size_t octant_index = 0; octant_index < 8; ++octant_index)
    {
        // Skip zero flags
        if (((b >> octant_index) & 1) == 0)
            continue;

        // Allocate new node
        root->octants[octant_index] = new octree_node;

        // Set the extent on the new node
        root->octants[octant_index]->e = get_new_extent (root->e, octant_index, midp);

        // Recurse into the new octant
        add_octree_octant (root->octants[octant_index], bytes, ++byte_index);
    }
}

class octree
{
    private:
    octree_node *root;

    void free_node (octree_node * const t)
    {
        assert (t != nullptr);

        // Free children first
        for (auto n : t->octants)
            if (n != nullptr)
                free_node (n);

        // Free the node
        delete t;
    }

    public:
    // CTOR
    octree (const size_t max_depth,
        const std::vector<point<double>> &points)
        : root (nullptr)
    {
        // Make sure we have data
        if (points.empty ())
            return;

        // Create the tree
        root = new octree_node;

        // Get the extent min/max points
        root->e = get_extent (points);

        // Add the points
        for (const auto &p : points)
            add_octree_point (root, p, max_depth, 0);
    }
    // CTOR
    octree (const std::vector<uint8_t> &bytes,
        const spc::extent<double> &e)
        : root (nullptr)
    {
        // Create the tree
        root = new octree_node;

        // Set the extent
        root->e = e;

        // Create the nodes from the encoded bytes
        size_t byte_index = 0;
        add_octree_octant (root, bytes, byte_index);
    }
    // DTOR
    ~octree ()
    {
        free_node (root);
    }
    const octree_node *get_root () const { return root; }
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
    unsigned leaf_points = 0;
    const auto node_function = [&] (const spc::octree_node *root)
    {
        ++nodes;
    };
    const auto leaf_function = [&] (const spc::octree_node *root)
    {
        ++leafs;
        leaf_points += root->total_points;
    };
    spc::dfs (o.get_root (), node_function, leaf_function);
    os << "extent " << o.get_root ()->e.maxp - o.get_root ()->e.minp
        << " nodes " << nodes
        << " leafs " << leafs
        << " leaf_points " << leaf_points
        << " points/leaf " << leaf_points * 1.0 / leafs << std::endl;
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
    std::uniform_int_distribution<int> e (min_exponent, max_exponent);

    for (auto &p : points)
    {
        const double x = std::ldexp (d (g), e (g));
        const double y = std::ldexp (d (g), e (g));
        const double z = std::ldexp (d (g), e (g));
        p = point<double> {x, y, z};
    }

    clog << "Encoding " << N << " random points" << endl;

    const size_t max_depth = 10;
    octree o (max_depth, points);

    print_info (clog, o);

    {
    const auto x = encode_extent (o.get_root ()->e);
    const auto e = decode_extent (x);
    verify (o.get_root ()->e == e);
    }

    {
    const auto x = encode_octree (o.get_root ());
    octree o2 (x, o.get_root ()->e);
    print_info (clog, o2);
    const auto y = encode_octree (o2.get_root ());
    verify (x == y);
    }

    return;

    {
    const auto x = encode_point_counts (o.get_root ());
    clog << "size " << x.size ();
    const auto y = spc::compress (x);
    clog << " compressed size " << y.size ();
    clog << " ratio = " << y.size () * 100.0 / x.size () << endl;
    const auto z = spc::decompress (y);
    verify (x == z);
    }

    {
    const auto x = encode_point_deltas (o.get_root ());
    clog << "size " << x.size ();
    const auto y = spc::compress (x);
    clog << " compressed size " << y.size ();
    clog << " ratio = " << y.size () * 100.0 / x.size () << endl;
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
        //test (10, 1, 10);
        //test (10, -10, 10);
        test (10'000, 0, 0);
        test (10'000, 0, 10);
        test (10'000, -10, 10);
        //test (10'000, 0, 20);
        //test (10'000, -20, 20);
        test (10'000);
        //test (1'000'000);
        //test (1'000'000, 0, 0);
        //test (1'000'000, -10, 10);
        //test (10'000'000);
        //test (11'000'000, 0, 20);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
