#pragma once

#include <cassert>
#include <stdexcept>
#include <sstream>

namespace spoc
{

namespace detail
{

inline void ThrowError (const char *e, const char *file, const unsigned line)
{
    std::stringstream s;
    s << "Contract failed in " << file << ", line " << line << ": " << e;
    throw std::runtime_error (s.str ());
}

void assert_true (const bool expr,
    const char *e,
    const char *file,
    const unsigned line)
{
    if (!expr)
        detail::ThrowError (e, __FILE__, __LINE__);
}

} // namespace detail

} // namespace spoc

#ifndef NDEBUG

#define REQUIRE(e) spoc::detail::assert_true (e, #e, __FILE__, __LINE__);
#define ENSURE(e) spoc::detail::assert_true (e, #e, __FILE__, __LINE__);

#else

#define REQUIRE(e) ((void) 0)
#define ENSURE(e) ((void) 0)

#endif
