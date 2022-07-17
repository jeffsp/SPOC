#pragma once

#include <cassert>
#include <stdexcept>
#include <sstream>

namespace spoc
{

namespace detail
{

inline void ContractFailed (const char *e, const char *file, const unsigned line)
{
    std::stringstream s;
    s << "Contract failed in " << file << ", line " << line << ": " << e;
    throw std::runtime_error (s.str ());
}

void test_contract (const bool flag,
    const char *e,
    const char *file,
    const unsigned line)
{
    if (!flag)
        detail::ContractFailed (e, file, line);
}

} // namespace detail

} // namespace spoc

#ifndef NDEBUG

#define REQUIRE(e) spoc::detail::test_contract (e, #e, __FILE__, __LINE__);
#define ENSURE(e) spoc::detail::test_contract (e, #e, __FILE__, __LINE__);

#else

#define REQUIRE(e) ((void) 0)
#define ENSURE(e) ((void) 0)

#endif
