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

unsigned get_octant_index (const point<double> &p,
    const point<double> &minp,
    const point<double> &midp,
    const point<double> &maxp)
{
    // Get node index = 0, 1, ... 7
    const unsigned index =
        ((p.x < midp.x) << 0) |
        ((p.y < midp.y) << 1) |
        ((p.z < midp.z) << 2);
    assert (index < 8);
    return index;
}

struct node
{
    // Arrays are not default initialized for standard types
    std::array<node *, 8> octants = { nullptr };
};

uint8_t octants_to_byte (const std::array<node *, 8> &octants)
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
    // Terminating case
    if (root == nullptr)
    {
        leaf_function ();
        return;
    }
    node_function (root);
    for (auto n : root->octants)
        dfs (n, node_function, leaf_function);
}

class octree
{
    private:
    node *root;
    size_t max_depth;
    point<double> min_extent;
    point<double> max_extent;
    size_t nodes;
    size_t leafs;

    void add (node *root,
        const point<double> &p,
        const point<double> &minp,
        const point<double> &maxp,
        const unsigned depth)
    {
        // Check logic
        assert (root != nullptr);
        assert (octant_less (minp, maxp));
        assert (octant_less (minp, p));
        assert (octant_less (p, maxp));

        // Terminal case
        if (depth == max_depth)
        {
            ++leafs;
            return;
        }

        // Get midpoint between min and max
        const point<double> midp {
            (minp.x + maxp.x) / 2.0,
            (minp.y + maxp.y) / 2.0,
            (minp.z + maxp.z) / 2.0};

        const unsigned n = get_octant_index (p, minp, midp, maxp);
        assert (n < root->octants.size ());

        // Allocate new node if needed
        if (root->octants[n] == nullptr)
        {
            root->octants[n] = new node;
            ++nodes;
        }

        auto current_minp = minp;
        auto current_maxp = maxp;
        change_extent (p, current_minp, midp, current_maxp);
        add (root->octants[n], p, current_minp, current_maxp, depth + 1);
    }
    void free_node (node *t)
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
        : max_depth (max_depth)
        , nodes (0)
        , leafs (0)
    {
        // Make sure we have data
        if (points.empty ())
            return;

        // Get extent min/max points
        auto m = get_extent (points);

        min_extent = m.first;
        max_extent = m.second;

        // Create the tree
        root = new node;
        ++nodes;

        // Add the points
        for (const auto &p : points)
            add (root, p, min_extent, max_extent, 0);
    }
    // DTOR
    ~octree ()
    {
        free_node (root);
    }
    std::ostream &info (std::ostream &os) const
    {
        os << "min = " << min_extent
            << " max = " << max_extent
            << " leafs = " << leafs
            << " nodes = " << nodes
            << std::endl;
        return os;
    }
    std::vector<uint8_t> encode ()
    {
        std::vector<uint8_t> x;
        const auto node_function = [&] (const node *root)
        {
            x.push_back (octants_to_byte (root->octants));
        };
        const auto leaf_function = [&] ()
        {
        };
        spc::dfs (root, node_function, leaf_function);
        return x;
    }
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

    o.info (clog);

    const auto x = o.encode ();
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
        test (10'000'000);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
