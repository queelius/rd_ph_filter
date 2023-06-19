#pragma once

#include <limits>
#include <vector>

namespace bernoulli {
/**
 * @brief Rate-distorted perfect hash filter models the concept of a
 * (immutable) Bernoulli set.
 *
 * As a type of bernoulli set, it has a fase positive rate and a false
 * negative rate. In this case, the false positive rate is an expectation
 * that may be specified precisely and the false negative rate is a function
 * of a specified time and space complexity.
 *
 * @tparam PH a type that models the concept of a rate-distrted perfect hash
 * function, Hashable -> [0,m], where m is the maximum hash value.
 */
template <typename PH>
struct rd_ph_filter {
    using hash_type = typename PH::H::hash_type;

    auto perfect_hash_fn() const { return ph; }
    auto hash_fn() const { return ph.hash_fn(); }

    template <typename I>
    static auto build_filter(PH const& ph, I begin, I end)
    {
        std::vector<hash_type> hashes(ph.max_hash() + 1);
        for (auto x = begin; x != end; ++x) hashes[ph(*x)] = hash_fn()(*x);
        return hashes;
    }

    /**
     * @brief Construct object from iterator range [begin,end),
     * which is viewed as representing a set (duplicates and
     * order does not matter).
     *
     * It takes an object `builder` that models the concept of a
     * Builder. It is a functor that accepts an iterator range [begin,end)
     * and constructs a PH object for it with a pre-specified set of
     * parameters.
     *
     * @param begin start of elements to build a filter for
     * @param end end of elements
     * @tparam I mdoels the concept of a forward iterator
     * @tparam Builder models the concept of a builder for PH
     */
    template <typename I, typename Builder>
    rd_ph_filter(I begin, I end, Builder builder)
        : ph(builder(begin, end))
        , hashes(build_filter(ph, begin, end))
    {
    }

    /**
     * @brief Test element x for membership in the set.
     * 
     * @tparam X 
     * @param x 
     * @return auto 
     */
    template <typename X>
    auto operator()(X const& x) const
    {
        return hashes[ph(x)] == hash_fn(x);
    }

    static auto fpr()
    {
        return 1.0 / std::numeric_limits<hash_type>::max();
    }

    auto fnr() const
    {
        return ph.error_rate() * (1 - fpr());
    }

    PH const ph;
    std::vector<hash_type> const hashes;
};

template <typename PH>
auto fpr(rd_ph_filter<PH> const&)
{
    return rd_ph_filter<PH>::fpr();
}

template <typename PH>
auto fnr(rd_ph_filter<PH> const& s)
{
    return s.fnr();
}

template <typename PH>
auto is_member(auto const& x, rd_ph_filter<PH> const& s)
{
    return s(x);
}

/**
 * @brief the equality predicate.
 *
 * Representational equality implies equality.
 * if Hashable(PH::H) is finite, then
 * different representations could be equal.
 * However, we make the simplifying assumption
 * that this is not the case.
 *
 * @tparam PH perfect hash function type
 * @param lhs left-hand-side of equality
 * @param rhs right-hand-side of equality
 */
template <typename PH>
auto operator==(rd_ph_filter<PH> const& lhs, rd_ph_filter<PH> const& rhs)
{
    return (lhs.ph == rhs.ph) && (lhs.hashes == rhs.hashes);
}

template <typename PH>
auto operator!=(rd_ph_filter<PH> const& lhs, rd_ph_filter<PH> const& rhs)
{
    return !(lhs == rhs);
}

template <typename PH>
auto operator<=(rd_ph_filter<PH> const& lhs, rd_ph_filter<PH> const& rhs)
{
    return lhs == rhs;
}

template <typename PH>
auto operator<(rd_ph_filter<PH> const&, rd_ph_filter<PH> const&)
{
    return false;
}

template <typename PH>
auto operator>=(rd_ph_filter<PH> const& lhs, rd_ph_filter<PH> const& rhs)
{
    return lhs == rhs;
}

template <typename PH>
auto operator>(rd_ph_filter<PH> const&, rd_ph_filter<PH> const&)
{
    return false;
}
}
