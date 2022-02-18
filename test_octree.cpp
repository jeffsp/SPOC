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
    // Get node index = 0, 1, ... 7
    const unsigned index =
        ((p.x < midp.x) << 0) |
        ((p.y < midp.y) << 1) |
        ((p.z < midp.z) << 2);
    assert (index < 8);
    return index;
}

struct node;
using octant_ptrs = std::array<node *, 8>;

struct node
{
    // Arrays are not default initialized for standard types
    octant_ptrs octants = { nullptr };
    extent<double> e;
    size_t points = 0;
};

uint8_t octants_to_byte (const octant_ptrs &octants)
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

template<typename NODE_FUNCTION, typename LEAF_FUNCTION>
void dfs (const node *root,
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

        // Can't be a leaf if it has chilren
        is_leaf = false;

        // Recurse into non-null branches
        dfs (n, node_function, leaf_function);
    }

    // Call leaf if needed
    if (is_leaf)
        leaf_function (root);
}

std::vector<uint8_t> encode (const node *root)
{
    std::vector<uint8_t> x;
    const auto node_function = [&] (const node *root)
    {
        x.push_back (octants_to_byte (root->octants));
    };
    const auto leaf_function = [&] (const node *root)
    {
    };
    spc::dfs (root, node_function, leaf_function);
    return x;
}

void add_octree_point (node * const root,
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
    ++root->points;

    // Terminal case
    if (current_depth == max_depth)
        return;

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
        root->octants[n] = new node;
        // Set the extents on the new node
        root->octants[n]->e = get_new_extent (root->e, p, midp);
    }

    add_octree_point (root->octants[n], p, max_depth, current_depth + 1);
}

class octree
{
    private:
    node *root;

    void free_node (node * const t)
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
        root = new node;

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
    const node *get_root () const { return root; }
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

    const size_t max_depth = 8;
    octree o (max_depth, points);

    unsigned nodes = 0;
    unsigned leafs = 0;
    unsigned leaf_points = 0;
    const auto node_function = [&] (const node *root)
    {
        ++nodes;
    };
    const auto leaf_function = [&] (const node *root)
    {
        ++leafs;
        leaf_points += root->points;
    };
    spc::dfs (o.get_root (), node_function, leaf_function);
    clog << "nodes " << nodes
        << " leafs " << leafs
        << " leaf_points " << leaf_points << endl;

    const auto x = encode (o.get_root ());
    clog << "size " << x.size () << endl;
    const auto y = spc::compress (x);
    clog << "compressed size " << y.size () << endl;
    clog << "ratio = " << y.size () * 100.0 / x.size () << endl;
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
        test (10'000, 0, 0);
        test (10'000'000);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
