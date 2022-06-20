/**
 * perfect_hash_filter<H> models a bernoulli set, a type of rate-distorted
 * set. In this case, the false positive rate is an expectation that may be
 * specified precisely and the false negative rate is a function of a
 * specified time and space complexity.
 *
 * Conceptually, it composes a perfect hash function and a bit matrix of
 * hashes. However, in practice, we simply use an array of hashes of
 * type size_t.
 */

#include <vector>

namespace bernoulli
{
    /**
     * PH : Hashable -> [0,max(PH)]
     * Default: Coder is identity on T, in which
     */
    template <typename PH>
    class rd_ph_filter
    {
    public:
        auto perfect_hash_fn() const { return ph; }
        auto hash_fn() const { return ph.hash_fn(); }

        template <typename I, typename Builder>
        rd_ph_filter(
            I begin,
            I end,
            size_t k,
            Builder builder) : k(k), ph(builder(begin, end)
        {
            hs.resize(ph.max());
            for (auto x = begin; x != end; ++x)
                hs[ph(*x)] == ph.hash_fn(*x) % k;
        }

        template <typename X>
        bool contains(X const & x) const
        {
            return hs[ph(x)] == ph.hash_fn(x) % k;
        }

        auto false_positive_rate() const
        {
            return std::pow(2.,-(double)k);
        }

        auto false_negative_rate() const
        {
            return ph.error_rate();
        }

    private:
        size_t k;
        PH ph;
        std::vector<hash_type> hs;
    };


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
    auto operator==(rd_ph_filter<PH> const & lhs,
                    rd_ph_filter<PH> const & rhs)
    {
        return (lhs.ph == rhs.ph) &&
               (lhs.hs == rhs.hs) &&
               (lhs.complement == rhs.complement);
    }

    template <typename PH>
    auto operator!=(rd_ph_filter<PH> const & lhs,
                    rd_ph_filter<PH> const & rhs)
    {
        return !(lhs==rhs);
    }

    template <typename PH>
    auto operator<=(rd_ph_filter<PH> const & lhs,
                    rd_ph_filter<PH> const & rhs)
    {
        return lhs == rhs;
    }

    template <typename PH>
    auto operator<(rd_ph_filter<PH> const &,
                   rd_ph_filter<PH> const &)
    {
        return false;
    }

    template <typename PH>
    auto operator>=(rd_ph_filter<PH> const & lhs,
                    rd_ph_filter<PH> const & rhs)
    {
        return lhs == rhs;
    }


    template <typename PH>
    auto operator>(rd_ph_filter<PH> const &,
                   rd_ph_filter<PH> const &)
    {
        return false;
    }
}
