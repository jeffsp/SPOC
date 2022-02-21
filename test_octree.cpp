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
    for (auto i : octants)
    {
        if (i != nullptr)
            b |= 1;
        b <<= 1;
    }
    return b;
}

std::vector<uint8_t> encode (const octree_node *root)
{
    std::vector<uint8_t> octant_bytes;
    std::vector<uint64_t> point_counts;
    std::vector<point<double>> point_deltas;
    const auto node_function = [&] (const octree_node *root)
    {
        // Encode the tree
        octant_bytes.push_back (to_byte (root->octants));
    };
    const auto leaf_function = [&] (const octree_node *root)
    {
        // Encode the point locations
        point_counts.push_back (root->points.size ());
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
    // Search the tree, savings values as we go...
    spc::dfs (root, node_function, leaf_function);
    // Output bytes
    std::vector<uint8_t> x;
    // Save the tree
    x.insert (x.end (), octant_bytes.begin (), octant_bytes.end ());
    // Save the point counts (NOT PORTABLE)
    const uint8_t *pc = reinterpret_cast<const uint8_t *> (&point_counts[0]);
    x.insert (x.end (), pc, pc + point_counts.size () * sizeof(double));
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

    // Get midpoint between min and max extents
    const point<double> midp {
        (root->e.minp.x + root->e.maxp.x) / 2.0,
        (root->e.minp.y + root->e.maxp.y) / 2.0,
        (root->e.minp.z + root->e.maxp.z) / 2.0};

    // Which octant does 'p' belong to?
    const unsigned n = get_octant_index (p, midp);

    // Allocate new node if needed
    if (root->octants[n] == nullptr)
    {
        root->octants[n] = new octree_node;
        // Set the extents on the new node
        root->octants[n]->e = get_new_extent (root->e, p, midp);
    }

    add_octree_point (root->octants[n], p, max_depth, current_depth + 1);
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
    octree (const size_t max_depth, const std::vector<point<double>> &points)
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
    // DTOR
    ~octree ()
    {
        free_node (root);
    }
    const octree_node *get_root () const { return root; }
};

} // namespace spc

void test (const size_t N,
    const int min_exponent = std::numeric_limits<double>::min_exponent,
    const int max_exponent = std::numeric_limits<double>::max_exponent)
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

    const size_t max_depth = 20;
    octree o (max_depth, points);

    unsigned nodes = 0;
    unsigned leafs = 0;
    unsigned leaf_points = 0;
    const auto node_function = [&] (const octree_node *root)
    {
        ++nodes;
    };
    const auto leaf_function = [&] (const octree_node *root)
    {
        ++leafs;
        leaf_points += root->total_points;
        //clog << setprecision(std::numeric_limits<double>::digits10);
        //clog << "delta " << root->e.maxp - root->e.minp << endl;
    };
    spc::dfs (o.get_root (), node_function, leaf_function);
    clog << "minp " << o.get_root ()->e.minp
        << " maxp " << o.get_root ()->e.maxp
        << " nodes " << nodes
        << " leafs " << leafs
        << " leaf_points " << leaf_points << endl;

    const auto x = encode (o.get_root ());
    clog << "size " << x.size () << endl;
    const auto y = spc::compress (x);
    clog << "compressed size " << y.size () << endl;
    clog << "ratio = " << y.size () * 100.0 / x.size () << endl;
    clog << "bytes/pt = " << y.size () * 1.0 / points.size () << endl;
    const auto z = spc::decompress (y);
    verify (x == z);
    clog << endl;
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
        test (10'000);
        test (10'000, 0, 20);
        test (1'000'000);
        test (1'000'000, 0, 0);
        test (1'000'000, -10, 10);
        test (10'000'000);
        test (10'000'000, 0, 20);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
