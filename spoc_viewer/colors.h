#ifndef COLORS_H
#define COLORS_H

#include "palette.h"
#include "spoc/spoc.h"

namespace spoc_viewer
{

namespace colors
{

/// @brief helper for setting point colors
template<typename T,typename U>
std::vector<palette::rgb_triplet> get_elevation_colors (const T &pc, const U &palette)
{
    // Setup colors
    assert (!palette.empty ());
    const double resolution = 1.0;
    const auto e = spoc::extent::get_extent (pc);
    double dz = e.maxp.z - e.minp.z;

    std::vector<palette::rgb_triplet> rgbs;

    for (size_t i = 0; i < pc.size (); ++i)
    {
        const double z = pc[i].z - e.minp.z;
        const unsigned index = round ((palette.size () - 1) * z / dz);
        assert (index < palette.size ());
        const unsigned r = palette[index][0];
        const unsigned g = palette[index][1];
        const unsigned b = palette[index][2];
        assert (r < 256);
        assert (g < 256);
        assert (b < 256);
        rgbs.push_back (palette::rgb_triplet {r, g, b});
    }

    return rgbs;
}

/// @brief helper for setting point colors
template<typename T,typename U>
std::vector<palette::rgb_triplet> get_intensity_colors (const T &pc, const U &palette)
{
    // Setup colors
    assert (!palette.empty ());

    // Put all intensities into a vector
    std::vector<uint16_t> intensities (pc.size ());

    for (size_t i = 0; i < pc.size (); ++i)
        intensities[i] = pc[i].i;

    // Sort by value
    sort (intensities.begin (), intensities.end ());

    // Get the max
    double max_intensity = intensities.back ();

    // ... but never go below a max intensity of 255
    max_intensity = std::max (255.0, max_intensity);

    std::vector<palette::rgb_triplet> rgbs;

    for (size_t i = 0; i < pc.size (); ++i)
    {
        unsigned index = (max_intensity == 0.0)
            ? 0
            : round ((palette.size () - 1) * pc[i].i / max_intensity);
        // It may go over if it's in the top 5%
        index = std::min (size_t (index), size_t (palette.size () - 1));
        const unsigned r = palette[index][0];
        const unsigned g = palette[index][1];
        const unsigned b = palette[index][2];
        assert (r < 256);
        assert (g < 256);
        assert (b < 256);
        rgbs.push_back (palette::rgb_triplet {r, g, b});
    }

    return rgbs;
}

/// @brief helper for setting point colors
template<typename T,typename U>
std::vector<palette::rgb_triplet> get_shaded_classification_colors (const T &pc, const U &palette)
{
    // Put all intensities into a vector
    std::vector<uint16_t> intensities (pc.size ());

    // Sort by value
    sort (intensities.begin (), intensities.end ());

    // Get the one at the 95% quantile boundary...
    double max_intensity = intensities[intensities.size () * 0.95];

    // ... but never go below a max intensity of 255
    max_intensity = std::max (255.0, max_intensity);

    std::vector<palette::rgb_triplet> rgbs;

    // Setup colors
    for (size_t i = 0; i < pc.size (); ++i)
    {
        size_t index = static_cast<size_t> (pc[i].c);
        if (index >= palette.size ())
            index = 0;
        // Scale it by the intensity clamped to 1.0
        const double scale = std::min (pc[i].i/ max_intensity, 0.9) + 0.1;
        const unsigned r = palette[index][0];
        const unsigned g = palette[index][1];
        const unsigned b = palette[index][2];
        // Keep the color constant, but change the luminance
        auto yuv = spoc_viewer::palette::rgb2yuv (spoc_viewer::palette::rgb_triplet {r, g, b});
        yuv[0] *= scale;
        auto rgb = spoc_viewer::palette::yuv2rgb (yuv);
        assert (rgb[0] < 256);
        assert (rgb[1] < 256);
        assert (rgb[2] < 256);
        rgbs.push_back (palette::rgb_triplet {rgb[0], rgb[1], rgb[2]});
    }

    return rgbs;
}

/// @brief helper for setting point colors
template<typename T,typename U>
std::vector<palette::rgb_triplet> get_classification_colors (const T &pc, const U &palette)
{
    std::vector<palette::rgb_triplet> rgbs;

    // Setup colors
    for (size_t i = 0; i < pc.size (); ++i)
    {
        size_t index = static_cast<size_t> (pc[i].c);
        if (index >= palette.size ())
            index = 0;
        const unsigned r = palette[index][0];
        const unsigned g = palette[index][1];
        const unsigned b = palette[index][2];
        assert (r < 256);
        assert (g < 256);
        assert (b < 256);
        rgbs.push_back (palette::rgb_triplet {r, g, b});
    }

    return rgbs;
}

/// @brief helper for setting point colors
template<typename T,typename U,typename V>
std::vector<palette::rgb_triplet> get_region_colors (const T &pc, const U &palette, V indexer)
{
    std::vector<palette::rgb_triplet> rgbs;

    // Setup colors
    for (size_t i = 0; i < pc.size (); ++i)
    {
        // The index is based on the classification and region id
        const unsigned classification = static_cast<int> (pc[i].c);
        const unsigned id = static_cast<int> (pc[i].p);
        // ID 0 is special, it means no region has been assigned
        size_t index = id == 0 ? 0 : indexer (classification, id);
        assert (index < palette.size ());
        const unsigned r = palette[index][0];
        const unsigned g = palette[index][1];
        const unsigned b = palette[index][2];
        assert (r < 256);
        assert (g < 256);
        assert (b < 256);
        rgbs.push_back (palette::rgb_triplet {r, g, b});
    }

    return rgbs;
}

/// @brief helper for setting point colors
template<typename T,typename U>
std::vector<palette::rgb_triplet> get_region_colors_random (const T &pc, const U &palette)
{
    std::vector<palette::rgb_triplet> rgbs;

    // Setup colors
    for (size_t i = 0; i < pc.size (); ++i)
    {
        const unsigned id = static_cast<int> (pc[i].p);
        size_t index = id % palette.size ();
        assert (index < palette.size ());
        const unsigned r = palette[index][0];
        const unsigned g = palette[index][1];
        const unsigned b = palette[index][2];
        assert (r < 256);
        assert (g < 256);
        assert (b < 256);
        rgbs.push_back (palette::rgb_triplet {r, g, b});
    }

    return rgbs;
}

/// @brief helper for setting point colors
template<typename T>
std::vector<palette::rgb_triplet> get_rgb_colors (const T &pc)
{
    // Get range of RGB's
    auto minrgb = 0;
    auto maxrgb = pc[0].r;

    for (size_t i = 0; i < pc.size (); ++i)
    {
        maxrgb = std::max (maxrgb, pc[i].r);
        maxrgb = std::max (maxrgb, pc[i].g);
        maxrgb = std::max (maxrgb, pc[i].b);
    }

    if (minrgb == maxrgb)
    {
        std::cerr << "WARNING: This point cloud does not contain RGB data" << std::endl;
        maxrgb = minrgb + 1;
    }

    assert (maxrgb > minrgb);
    const double range = maxrgb - minrgb;
    const double scale = 256.0 / range - std::numeric_limits<float>::epsilon();

    std::vector<palette::rgb_triplet> rgbs;

    // Setup colors
    for (size_t i = 0; i < pc.size (); ++i)
    {
        const unsigned r = static_cast<unsigned> ((pc[i].r - minrgb) * scale);
        const unsigned g = static_cast<unsigned> ((pc[i].g - minrgb) * scale);
        const unsigned b = static_cast<unsigned> ((pc[i].b - minrgb) * scale);
        assert (r < 256);
        assert (g < 256);
        assert (b < 256);
        rgbs.push_back (palette::rgb_triplet {r, g, b});
    }

    return rgbs;
}

/// @brief helper for setting point colors
template<typename T,typename U>
std::vector<palette::rgb_triplet> get_extra_field_colors (const T &pc, const U &palette, const size_t n)
{
    // Setup colors
    assert (!palette.empty ());

    std::vector<palette::rgb_triplet> rgbs;

    if (pc.empty ())
        return rgbs;

    // Check the field
    if (n > pc[0].extra.size ())
        throw std::runtime_error ("This point cloud has no extra field #" + std::to_string(n));

    // Set a sentinal
    size_t min_x = pc[0].extra[n];
    size_t max_x = pc[0].extra[n];

    // Get min and max
    for (const auto p : pc)
    {
        min_x = std::min (p.extra[n], min_x);
        max_x = std::max (p.extra[n], max_x);
    }

    const size_t dx = max_x - min_x;

    for (size_t i = 0; i < pc.size (); ++i)
    {
        // All extra fields should be the same size
        assert (n < pc[i].extra.size ());
        const double x = pc[i].extra[n] - min_x;
        const unsigned index = dx == 0 ? 0 : round ((palette.size () - 1) * x / dx);
        assert (index < palette.size ());
        const unsigned r = palette[index][0];
        const unsigned g = palette[index][1];
        const unsigned b = palette[index][2];
        assert (r < 256);
        assert (g < 256);
        assert (b < 256);
        rgbs.push_back (palette::rgb_triplet {r, g, b});
    }

    return rgbs;
}

} // namespace colors

} // namespace spoc_viewer
  //
#endif // COLORS_H
