#include <array>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

template<typename T>
struct point { T x, y, z; };

bool operator== (const point<double> &a, const point<double> &b)
{
    if (a.x != b.x) return false;
    if (a.y != b.y) return false;
    if (a.z != b.z) return false;
    return true;
}

bool operator!= (const point<double> &a, const point<double> &b)
{
    return !(a == b);
}

// This is NOT a sorting operator
bool operator<= (const point<double> &a, const point<double> &b)
{
    if (a.x > b.x) return false;
    if (a.y > b.y) return false;
    if (a.z > b.z) return false;
    return true;
}

// This is NOT a sorting operator
bool operator>= (const point<double> &a, const point<double> &b)
{
    if (a.x < b.x) return false;
    if (a.y < b.y) return false;
    if (a.z < b.z) return false;
    return true;
}

std::ostream &operator<< (std::ostream &s, const point<double> &p)
{
    s << ' ' << p.x;
    s << ' ' << p.y;
    s << ' ' << p.z;
    return s;
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

unsigned get_node_index (const point<double> &p,
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

void change_extent (const point<double> &p,
    point<double> &minp,
    const point<double> &midp,
    point<double> &maxp)
{
    if (p.x < midp.x) maxp.x = midp.x; else minp.x = midp.x;
    if (p.y < midp.y) maxp.y = midp.y; else minp.y = midp.y;
    if (p.z < midp.z) maxp.z = midp.z; else minp.z = midp.z;
}

class octree
{
    private:
    node *root;
    size_t max_depth;
    size_t nodes;
    size_t leafs;

    void add (node *root,
        const point<double> &p,
        const point<double> &minp,
        const point<double> &maxp,
        const int depth)
    {
        // Check logic
        assert (root != nullptr);
        assert (minp <= maxp);
        assert (p >= minp);
        assert (p <= maxp);

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
    void free_node (node *t)
    {
        assert (t != nullptr);

        // Free children first
        for (auto n : t->nodes)
            if (n != nullptr)
                free_node (n);

        // Free the node
        delete t;
    }
    uint8_t encode (const std::array<node *, 8> &nodes)
    {
        uint8_t b = 0;
        for (auto i : nodes)
        {
            if (i != nullptr)
                b |= 1;
            b <<= 1;
        }
        return b;
    }
    void encode (const node *root, std::vector<uint8_t> &x)
    {
        // Terminating case
        if (root == nullptr)
            return;
        x.push_back (encode (root->nodes));
        for (auto n : root->nodes)
            encode (n, x);
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

        // Create the tree
        root = new node;
        ++nodes;

        // Add the points
        for (const auto &p : points)
            add (root, p, m.first, m.second, 0);
    }
    // DTOR
    ~octree ()
    {
        free_node (root);
    }
    std::ostream &info (std::ostream &os) const
    {
        os << " leafs = " << leafs
            << " nodes = " << nodes
            << std::endl;
        return os;
    }
    std::vector<uint8_t> encode ()
    {
        std::vector<uint8_t> x;
        encode (root, x);
        return x;
    }
};


int main (int argc, char **argv)
{
    using namespace std;

    try
    {
        const size_t N = 10'000;

        clog << "Generating " << N << " random points" << endl;

        vector<point<double>> points (N);

        // Generate random doubles at varying scales.
        //
        // We don't want a uniform distribution.
        std::default_random_engine g;
        std::uniform_real_distribution<double> d (0.0, 1.0);
        std::uniform_int_distribution<int> e (
            std::numeric_limits<double>::min_exponent,
            std::numeric_limits<double>::max_exponent);

        for (auto &p : points)
        {
            const double x = std::ldexp (d (g), e (g));
            const double y = std::ldexp (d (g), e (g));
            const double z = std::ldexp (d (g), e (g));
            p = point<double> {x, y, z};
        }

        clog << "Encoding " << N << " random points" << endl;

        const size_t max_depth = 32;
        octree o (max_depth, points);

        o.info (clog);
        clog << endl;

        const auto x = o.encode ();
        clog << "size " << x.size () << endl;

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}
