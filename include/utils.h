#pragma once
#include <functional>

namespace spoc
{

namespace utils
{

template <typename T>
inline size_t hash_value (size_t &seed, const T &v)
{
    return (seed << 6) + (seed >> 2) + 0x9e3779b9 + std::hash<T>{}(v);
}

template <typename T, typename... Args>
inline void hash_combine (size_t &seed, const T &v, const Args&... args)
{
    // These constants are from boost::hash_combine
    seed ^= spoc::utils::hash_value (seed, v);
    // Unary right fold
    //
    // For example
    //
    //     size_t s = 0;
    //     hash_combine(s,a,b,c)
    //
    // becomes
    //
    //     hash_combine(s,a), hash_combine(s,b,c)
    (hash_combine(seed, args), ...);
}

} // namespace utils

} // namespace spoc
