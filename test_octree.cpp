#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include "spc.h"

struct pointf { double x, y, z; };

struct pointi { int64_t x, y, z; };

pointi ftoi (const pointf &p, const pointf &s)
{
    const uint64_t c = (1UL << 51);
    pointi q;
    q.x = std::round (p.x / s.x * c);
    q.y = std::round (p.y / s.y * c);
    q.z = std::round (p.z / s.z * c);
    return q;
}

pointf itof (const pointi &p, const pointf &s)
{
    const uint64_t c = (1UL << 51);
    pointf q;
    q.x = static_cast<double> (p.x) / c * s.x;
    q.y = static_cast<double> (p.y) / c * s.y;
    q.z = static_cast<double> (p.z) / c * s.z;
    return q;
}

bool operator== (const pointi &a, const pointi &b)
{
    if (a.x != b.x) return false;
    if (a.y != b.y) return false;
    if (a.z != b.z) return false;
    return true;
}

bool operator!= (const pointi &a, const pointi &b)
{
    return !(a == b);
}

bool operator<= (const pointi &a, const pointi &b)
{
    if (a.x > b.x) return false;
    if (a.y > b.y) return false;
    if (a.z > b.z) return false;
    return true;
}

bool operator>= (const pointi &a, const pointi &b)
{
    if (a.x < b.x) return false;
    if (a.y < b.y) return false;
    if (a.z < b.z) return false;
    return true;
}

struct node
{
    // Arrays are not default initialized for standard types
    std::array<node *, 8> nodes = { nullptr };
};

template<typename T>
std::pair<pointi, pointi> get_extent (const T &points)
{
    pointi minp { std::numeric_limits<int64_t>::max (),
        std::numeric_limits<int64_t>::max (),
        std::numeric_limits<int64_t>::max ()};
    pointi maxp { std::numeric_limits<int64_t>::lowest (),
        std::numeric_limits<int64_t>::lowest (),
        std::numeric_limits<int64_t>::lowest ()};
    for (const auto &p : points)
    {
        minp.x = std::min (p.x, minp.x);
        minp.y = std::min (p.y, minp.y);
        minp.z = std::min (p.z, minp.z);
        maxp.x = std::max (p.x, maxp.x);
        maxp.y = std::max (p.y, maxp.y);
        maxp.z = std::max (p.z, maxp.z);
    }
    return {minp, maxp};
}

template<typename T>
pointf get_scale (const T &points)
{
    pointf scalep { std::numeric_limits<double>::lowest (),
        std::numeric_limits<double>::lowest (),
        std::numeric_limits<double>::lowest ()};
    for (const auto &p : points)
    {
        scalep.x = std::max (std::fabs(p.x), scalep.x);
        scalep.y = std::max (std::fabs(p.y), scalep.y);
        scalep.z = std::max (std::fabs(p.z), scalep.z);
    }
    return scalep;
}

unsigned get_node_index (const pointi &p,
    const pointi &minp,
    const pointf &midp,
    const pointi &maxp)
{
    // Get node index = 0, 1, ... 7
    const unsigned index =
        ((p.x < midp.x) << 0) |
        ((p.y < midp.y) << 1) |
        ((p.z < midp.z) << 2);
    assert (index < 8);
    return index;
}

void change_extent (const pointi &p,
    pointi &minp,
    const pointf &midp,
    pointi &maxp)
{
    if (p.x < midp.x)
        maxp.x = std::floor (midp.x);
    else
        minp.x = std::ceil (midp.x);
    if (p.y < midp.y)
        maxp.y = std::floor (midp.y);
    else
        minp.y = std::ceil (midp.y);
    if (p.z < midp.z)
        maxp.z = std::floor (midp.z);
    else
        minp.z = std::ceil (midp.z);
}

class octree
{
    private:
    pointf scale;
    node root;
    size_t nodes;
    size_t leafs;

    void add (node *root,
        const pointi &p,
        const pointi &minp,
        const pointi &maxp,
        const int depth)
    {
        // Check logic
        assert (root != nullptr);
        assert (minp <= maxp);
        assert (p >= minp);
        assert (p <= maxp);

        // Terminal case
        if (minp == maxp)
        {
            ++leafs;
            std::clog << "depth = " << depth
                << " leafs = " << leafs
                << " nodes = " << nodes
                << std::endl;
            return;
        }

        // Get midpoint between min and max
        const pointf midp {
            (minp.x + maxp.x) / 2.0,
            (minp.y + maxp.y) / 2.0,
            (minp.z + maxp.z) / 2.0};

        const unsigned n = get_node_index (p, minp, midp, maxp);
        assert (n < root->nodes.size ());

        // Allocate new node if needed
        if (root->nodes[n] == nullptr)
        {
            root->nodes[n] = new node;
            ++nodes;
        }

        auto current_minp = minp;
        auto current_maxp = maxp;
        change_extent (p, current_minp, midp, current_maxp);
        add (root->nodes[n], p, current_minp, current_maxp, depth + 1);
    }

    public:
    // CTOR
    octree (const std::vector<pointf> &pfs)
        : nodes (0)
        , leafs (0)
    {
        // Make sure we have data
        if (pfs.empty ())
            return;

        // Determine the dynamic range in each dimension
        scale = get_scale (pfs);

        // Convert points to integers
        std::vector<pointi> pis (pfs.size ());

        for (size_t i = 0; i < pis.size (); ++i)
            pis[i] = ftoi (pfs[i], scale);

        // Get extent min/max points
        auto m = get_extent (pis);
        auto minp = m.first;
        auto maxp = m.second;

        // Create the tree
        for (const auto &p : pis)
            add (&root, p, minp, maxp, 0);
    }
    // DTOR
    ~octree ()
    {
        for (auto n : root.nodes)
            free_node (n);
    }
    void free_node (node *t)
    {
        // Terminating condition
        if (t == nullptr)
            return;

        // Free children first
        for (auto n : t->nodes)
            free_node (n);

        // Free the node
        delete t;
    }
};

std::vector<uint8_t> encode (const octree &t)
{
    std::vector<uint8_t> x;
    return x;
}

std::vector<pointf> decode (const std::vector<uint8_t> &t)
{
    std::vector<pointf> p;
    return p;
}

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spc;

    try
    {
        clog << "Reading point records" << endl;

        // Read SRS
        string wkt;
        getline (cin, wkt);

        // Read points
        vector<pointf> points;

        for (string line; getline (cin, line ); )
        {
            // Parse the line
            stringstream ss (line);
            pointf p;
            ss >> p.x; ss >> p.y; ss >> p.z;
            points.push_back (p);
        }

        clog << points.size () << " points read" << endl;


        clog << "Creating octree" << endl;
        octree t (points);

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
