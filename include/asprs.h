#pragma once
#include "compression.h"
#include "point.h"
#include "version.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_set>
#include <vector>

namespace spoc
{

/// Definitions from the ASPRS LAS 1.4 specification
namespace asprs
{

enum class point_class
{
    unclassified = 0,
    unknown = 1,
    ground = 2,
    low_veg = 3,
    med_veg = 4,
    high_veg = 5,
    building = 6,
    low_noise = 7,
    reserved1 = 8,
    water = 9,
    rail = 10,
    road = 11,
    reserved2 = 12,
    wire_guard = 13,
    wire_conductor = 14,
    transmission_tower = 15,
    wire_connector = 16,
    bridge_deck = 17,
    high_noise = 18,
    overhead_structure = 19,
    ignored = 20,
    snow = 21,
    exclusion = 22,
};

std::unordered_map<int,std::string> get_asprs_class_map ()
{
    std::unordered_map<int,std::string> m;
    m[0] = "unclassified";
    m[1] = "unknown";
    m[2] = "ground";
    m[3] = "low_veg";
    m[4] = "med_veg";
    m[5] = "high_veg";
    m[6] = "building";
    m[7] = "low_noise";
    m[8] = "reserved";
    m[9] = "water";
    m[10] = "rail";
    m[11] = "road";
    m[12] = "reserved";
    m[13] = "wire_guard";
    m[14] = "wire_conductor";
    m[15] = "transmission_tower";
    m[16] = "wire_connector";
    m[17] = "bridge_deck";
    m[18] = "high_noise";
    m[19] = "overhead_structure";
    m[20] = "ignored";
    m[21] = "snow";
    m[22] = "exclusion";
    for (int i = 23; i < 64; ++i)
        m[i] = "reserved";
    for (int i = 64; i < 256; ++i)
        m[i] = "user_defined";
    return m;
}

} // namespace asprs

} // namespace spoc
