#pragma once

#include <array>
#include <cassert>
#include <limits>
#include <memory>
#include <vector>
#include "extent.h"

namespace spc
{

// A node in an octree represents a cube in 3D space. The cube is
// divided into 8 octants. An array of 8 pointers recursively divides
// the node into subspaces.
struct octree_node
{
    extent<double> e;
    std::array<std::unique_ptr<octree_node>, 8> octants;
    std::unique_ptr<std::vector<point<double>>> deltas;
};

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
inline unsigned get_octant_index (const point<double> &p, const point<double> &ref)
{
    // Get octree node index = 0..7
    const unsigned index =
        ((p.x < ref.x) << 0) |
        ((p.y < ref.y) << 1) |
        ((p.z < ref.z) << 2);
    assert (index < 8);
    return index;
}

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

inline void add_octree_point (const std::unique_ptr<octree_node> &node,
    const point<double> &p,
    const unsigned max_depth,
    const unsigned current_depth)
{
    // Check logic
    assert (node != nullptr);
    assert (all_less_equal (node->e.minp, node->e.maxp));
    assert (all_less_equal (node->e.minp, p));
    assert (all_less_equal (p, node->e.maxp));

    // Terminal case
    if (current_depth == max_depth)
    {
        // Allocate delta container if needed
        if (node->deltas == nullptr)
            node->deltas.reset (new std::vector<point<double>>);

        // Get the octant corner point
        const auto q = node->e.minp;

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
        (node->e.minp.x + node->e.maxp.x) / 2.0,
        (node->e.minp.y + node->e.maxp.y) / 2.0,
        (node->e.minp.z + node->e.maxp.z) / 2.0};

    // Which octant does 'p' belong to?
    const unsigned octant_index = get_octant_index (p, midp);

    // Allocate new node if needed
    if (node->octants[octant_index] == nullptr)
    {
        node->octants[octant_index].reset (new octree_node);
        // Get the extent on the new node
        node->octants[octant_index]->e = get_new_extent (node->e, p, midp);
    }

    add_octree_point (node->octants[octant_index], p, max_depth, current_depth + 1);
}

inline void add_octree_octant (const std::unique_ptr<octree_node> &node,
    const std::vector<uint8_t> &octant_bytes,
    size_t &byte_index)
{
    // Check logic
    assert (node != nullptr);
    assert (byte_index <= octant_bytes.size ());
    assert (all_less_equal (node->e.minp, node->e.maxp));

    // Terminating case
    if (byte_index == octant_bytes.size ())
        return;

    // Get the octant flags
    const uint8_t b = octant_bytes[byte_index];

    // Is it a leaf?
    if (b == 0)
        return;

    // Get midpoint between min and max extent
    const point<double> midp {
        (node->e.minp.x + node->e.maxp.x) / 2.0,
        (node->e.minp.y + node->e.maxp.y) / 2.0,
        (node->e.minp.z + node->e.maxp.z) / 2.0};

    // How many octants are occupied in this extent?
    for (size_t octant_index = 0; octant_index < 8; ++octant_index)
    {
        // Skip zero flags
        if (((b >> octant_index) & 1) == 0)
            continue;

        // Allocate new node
        node->octants[octant_index].reset (new octree_node);

        // Get the extent of the new node
        node->octants[octant_index]->e = get_new_extent (node->e, octant_index, midp);

        // Recurse into the new octant
        add_octree_octant (node->octants[octant_index], octant_bytes, ++byte_index);
    }
}

template<typename T>
uint8_t octants_to_byte (const T &octants)
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

inline std::vector<uint8_t> encode_octants (const std::unique_ptr<octree_node> &root)
{
    std::vector<uint8_t> x;
    const auto node_function = [&] (const std::unique_ptr<octree_node> &node)
    {
        // Encode the octant pointers
        x.push_back (octants_to_byte (node->octants));
    };
    const auto leaf_function = [&] (const std::unique_ptr<octree_node> &node) { };
    spc::dfs (root, node_function, leaf_function);
    return x;
}

inline std::vector<uint64_t> get_point_counts (const std::unique_ptr<octree_node> &root)
{
    std::vector<uint64_t> point_counts;
    const auto node_function = [&] (const std::unique_ptr<octree_node> &node) { };
    const auto leaf_function = [&] (const std::unique_ptr<octree_node> &node)
    {
        // Check logic
        assert (node != nullptr);
        // Save the point count
        point_counts.push_back (node->deltas->size ());
    };
    spc::dfs (root, node_function, leaf_function);
    return point_counts;
}

inline std::vector<uint8_t> encode_point_counts (const std::vector<uint64_t> &point_counts)
{
    // Output bytes
    std::vector<uint8_t> x;
    // Encode the point counts (NOT PORTABLE)
    const uint8_t *pc = reinterpret_cast<const uint8_t *> (&point_counts[0]);
    x.insert (x.end (), pc, pc + point_counts.size () * sizeof(uint64_t));
    return x;
}

inline std::vector<uint64_t> decode_point_counts (const std::vector<uint8_t> &x)
{
    // Check logic
    assert (x.size () % sizeof(uint64_t) == 0);

    // Decoded counts
    std::vector<uint64_t> point_counts;
    // Decode the point counts (NOT PORTABLE)
    const uint64_t *pc = reinterpret_cast<const uint64_t *> (&x[0]);
    const size_t total_points = x.size () / sizeof(uint64_t);
    point_counts.insert (point_counts.end (), pc, pc + total_points);
    return point_counts;
}

inline std::vector<point<double>> get_point_deltas (const std::unique_ptr<octree_node> &root)
{
    std::vector<point<double>> point_deltas;
    const auto node_function = [&] (const std::unique_ptr<octree_node> &node) { };
    const auto leaf_function = [&] (const std::unique_ptr<octree_node> &node)
    {
        // Check logic
        assert (node != nullptr);
        // Save the point location
        point_deltas.insert (point_deltas.end (),
            node->deltas->begin (),
            node->deltas->end ());
    };
    spc::dfs (root, node_function, leaf_function);
    return point_deltas;
}

inline std::vector<uint8_t> encode_point_deltas (const std::vector<point<double>> &point_deltas)
{
    // Output bytes
    std::vector<uint8_t> x;
    // Encode the point deltas (NOT PORTABLE)
    const uint8_t *pd = reinterpret_cast<const uint8_t *> (&point_deltas[0]);
    x.insert (x.end (), pd, pd + point_deltas.size () * sizeof(point<double>));
    return x;
}

inline std::vector<point<double>> decode_point_deltas (const std::vector<uint8_t> &x)
{
    // Check logic
    assert (x.size () % sizeof(point<double>) == 0);

    // Decoded deltas
    std::vector<point<double>> point_deltas;
    // Decode the point deltas (NOT PORTABLE)
    const point<double> *pc = reinterpret_cast<const point<double> *> (&x[0]);
    const size_t total_points = x.size () / sizeof(point<double>);
    point_deltas.insert (point_deltas.end (), pc, pc + total_points);
    return point_deltas;
}

struct encoded_octree
{
    std::vector<uint8_t> extent_bytes;
    std::vector<uint8_t> octant_bytes;
    std::vector<uint8_t> point_count_bytes;
    std::vector<uint8_t> point_delta_bytes;
};

class octree
{
    private:
    std::unique_ptr<octree_node> root;

    public:
    // Contruct an octree from a container of 3D points
    octree (const size_t max_depth, const std::vector<point<double>> &points)
    {
        // Make sure we have data
        if (points.empty ())
            return;

        // Allocate the root node
        root.reset (new octree_node);

        // Get the extent min/max points
        root->e = get_extent (points);

        // Add the points
        for (const auto &p : points)
            add_octree_point (root, p, max_depth, 0);
    }
    // Construct an octree over a given extent from encoded bytes
    octree (const std::vector<uint8_t> &extent_bytes,
        const std::vector<uint8_t> &octant_bytes)
    {
        // Create the tree
        root.reset (new octree_node);

        // Get the extent
        root->e = decode_extent (extent_bytes);

        // Create the nodes from the encoded bytes
        size_t byte_index = 0;
        add_octree_octant (root, octant_bytes, byte_index);
    }
    // Construct an octree over a given extent from encoded bytes
    octree (const encoded_octree &x)
    {
        // Create the tree
        root.reset (new octree_node);

        // Get the extent
        root->e = decode_extent (x.extent_bytes);

        // Create the nodes from the encoded bytes
        size_t byte_index = 0;
        add_octree_octant (root, x.octant_bytes, byte_index);

        // Get the point counts
        const auto pcs = decode_point_counts (x.point_count_bytes);

        // Get the point deltas
        const auto pds = decode_point_deltas (x.point_delta_bytes);

        // Set the points
        size_t pcs_index = 0;
        size_t pds_index = 0;
        const auto node_function = [&] (const std::unique_ptr<spc::octree_node> &node) { };
        const auto leaf_function = [&] (const std::unique_ptr<spc::octree_node> &node)
        {
            // Check logic
            assert (node->deltas == nullptr);
            assert (pcs_index < pcs.size ());
            // Allocate delta container
            node->deltas.reset (new std::vector<point<double>>);

            // Resize the delta container
            node->deltas->resize (pcs[pcs_index++]);
            // Add the deltas
            for (size_t i = 0; i < node->deltas->size (); ++i)
            {
                assert (pds_index < pds.size ());
                (*node->deltas)[i] = pds[pds_index++];
            }
        };
        dfs (root, node_function, leaf_function);
    }
    const std::unique_ptr<octree_node> &get_root () const { return root; }
    std::vector<point<double>> get_points () const
    {
        // Count the total points
        unsigned total_points = 0;
        const auto node_function = [&] (const std::unique_ptr<spc::octree_node> &node) { };
        const auto leaf_function1 = [&] (const std::unique_ptr<spc::octree_node> &node)
        {
            assert (node->deltas != nullptr);
            total_points += node->deltas->size ();
        };
        dfs (root, node_function, leaf_function1);

        // Get the point values
        std::vector<point<double>> p;
        p.reserve (total_points);
        const auto leaf_function2 = [&] (const std::unique_ptr<spc::octree_node> &node)
        {
            // Get the octant corner point
            const auto q = node->e.minp;
            // Add the points contained in this leaf node
            for (size_t i = 0; i < node->deltas->size (); ++i)
                p.push_back ((*node->deltas)[i] + q);
        };
        dfs (root, node_function, leaf_function2);

        // Check our logic
        assert (p.size () == total_points);
        return p;
    }
};

inline encoded_octree encode (const octree &o)
{
    encoded_octree x;

    x.extent_bytes = encode_extent (o.get_root ()->e);
    x.octant_bytes = encode_octants (o.get_root ());
    x.point_count_bytes = encode_point_counts (get_point_counts (o.get_root ()));
    x.point_delta_bytes = encode_point_deltas (get_point_deltas (o.get_root ()));
    return x;
}


inline void print_octree_info (std::ostream &os, const spc::octree &o)
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
    dfs (o.get_root (), node_function, leaf_function);
    os << " nodes " << nodes
        << " leafs " << leafs
        << " total_points " << total_points
        << " points/leaf " << total_points * 1.0 / leafs << std::endl;
}

inline void print_octree (std::ostream &os, const spc::octree &o)
{
    size_t node_number = 0;
    const auto node_function = [&] (const std::unique_ptr<spc::octree_node> &node)
    {

        os << node_number++
            << "\tmin " << node->e.minp
            << "\tmax " << node->e.maxp
            << std::endl;
    };
    const auto leaf_function = [&] (const std::unique_ptr<spc::octree_node> &node)
    {
    };
    dfs (o.get_root (), node_function, leaf_function);
}

} // namespace spc
