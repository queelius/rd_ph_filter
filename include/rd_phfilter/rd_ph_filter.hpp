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

        /**
         * @param begin start of elements to build a filter for
         * @param end end of elements
         * @param k false positive rate is given by 2^(-k)
         * @tparam I mdoels the concept of a forward iterator
         * @tparam Builder models the concept of a builder for PH
         */
        template <typename I, typename Builder>
        rd_ph_filter(
            I begin,
            I end,
            size_t k,
            Builder builder) : k(k), ph(builder(begin, end))
        {
            hs.resize(ph.max());
            for (auto x = begin; x != end; ++x)
                hs[ph(*x)] = hash_fn(*x) % k;
        }

        template <typename X>
        auto contains(X const & x) const
        {
            return hs[ph(x)] == hash_fn(x) % k;
        }

        auto false_positive_rate() const
        {
            return std::pow(2.,-(double)k);
        }

        auto false_negative_rate() const
        {
            return ph.error_rate()*(1-false_positive_rate());
        }

    private:
        size_t const k;
        PH const ph;
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
               (lhs.hs == rhs.hs);
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
