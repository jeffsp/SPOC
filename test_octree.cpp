#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <sstream>
#include "spc.h"

template<typename T>
struct point { T x, y, z; };

struct spc_text_header
{
    std::string wkt;
    point<double> xyz_scale_factor;
    point<double> xyz_offset;
};

struct spc_text_header read_spc_text_header (std::istream &is)
{
    spc_text_header h;
    std::getline (is, h.wkt);

    std::string line;
    // Read the xyz scale
    {
        std::getline (is, line);
        std::stringstream ss (line);
        ss >> h.xyz_scale_factor.x;
        ss >> h.xyz_scale_factor.y;
        ss >> h.xyz_scale_factor.z;
    }
    // Read the xyz offset
    {
        std::getline (is, line);
        std::stringstream ss (line);
        ss >> h.xyz_offset.x;
        ss >> h.xyz_offset.y;
        ss >> h.xyz_offset.z;
    }

    return h;
}

bool operator== (const point<int64_t> &a, const point<int64_t> &b)
{
    if (a.x != b.x) return false;
    if (a.y != b.y) return false;
    if (a.z != b.z) return false;
    return true;
}

bool operator!= (const point<int64_t> &a, const point<int64_t> &b)
{
    return !(a == b);
}

bool operator<= (const point<int64_t> &a, const point<int64_t> &b)
{
    if (a.x > b.x) return false;
    if (a.y > b.y) return false;
    if (a.z > b.z) return false;
    return true;
}

bool operator>= (const point<int64_t> &a, const point<int64_t> &b)
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
std::pair<point<T>, point<T>> get_extent (const std::vector<point<T>> &points)
{
    point<T> minp { std::numeric_limits<T>::max (),
        std::numeric_limits<T>::max (),
        std::numeric_limits<T>::max ()};
    point<T> maxp { std::numeric_limits<T>::lowest (),
        std::numeric_limits<T>::lowest (),
        std::numeric_limits<T>::lowest ()};
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

unsigned get_node_index (const point<int64_t> &p,
    const point<int64_t> &minp,
    const point<double> &midp,
    const point<int64_t> &maxp)
{
    // Get node index = 0, 1, ... 7
    const unsigned index =
        ((p.x < midp.x) << 0) |
        ((p.y < midp.y) << 1) |
        ((p.z < midp.z) << 2);
    assert (index < 8);
    return index;
}

void change_extent (const point<int64_t> &p,
    point<int64_t> &minp,
    const point<double> &midp,
    point<int64_t> &maxp)
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
    node root;
    size_t nodes;
    size_t leafs;

    void add (node *root,
        const point<int64_t> &p,
        const point<int64_t> &minp,
        const point<int64_t> &maxp,
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
        const point<double> midp {
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
    octree (const std::vector<point<int64_t>> &points)
        : nodes (0)
        , leafs (0)
    {
        // Make sure we have data
        if (points.empty ())
            return;

        // Get extent min/max points
        auto m = get_extent (points);

        // Create the tree
        for (const auto &p : points)
            add (&root, p, m.first, m.second, 0);
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

std::vector<int64_t> decode (const std::vector<uint8_t> &t)
{
    std::vector<int64_t> p;
    return p;
}

int main (int argc, char **argv)
{
    using namespace std;
    using namespace spc;

    try
    {
        clog << "Reading point records" << endl;

        // Read header
        const spc_text_header h = read_spc_text_header (cin);

        clog << fixed;
        clog << setprecision(std::numeric_limits<double>::digits10);
        clog << h.wkt << endl;
        clog << h.xyz_scale_factor.x
            << "\t" << h.xyz_scale_factor.y
            << "\t" << h.xyz_scale_factor.z << endl;
        clog << h.xyz_offset.x
            << "\t" << h.xyz_offset.y
            << "\t" << h.xyz_offset.z << endl;

        // Read points
        vector<point<int64_t>> points;

        for (string line; getline (cin, line); )
        {
            // Parse the line
            stringstream ss (line);
            point<int64_t> p;
            ss >> p.x;
            ss >> p.y;
            ss >> p.z;
            points.push_back (p);
        }

        clog << points.size () << " points read" << endl;

        // Subtract off minimum
        auto m = get_extent (points);
        for (auto &p : points)
        {
            p.x -= m.first.x;
            p.y -= m.first.y;
            p.z -= m.first.z;
        }
        m = get_extent (points);

        clog << "Creating octree" << endl;
        octree t (points);

        clog << points.size () << " points processed" << endl;

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
