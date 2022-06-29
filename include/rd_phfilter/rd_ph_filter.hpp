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
#include <limits>

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

        static auto build_filter(PH const & ph, I begin, I end)
        {
            std::vector<hash_type> hashes(ph.max_hash()+1);
            for (auto x = begin; x != end; ++x)
                hashes[ph(*x)] = ph.hash_fn(*x);
            return hashes;
        }

        /**
         * @param begin start of elements to build a filter for
         * @param end end of elements
         * @tparam I mdoels the concept of a forward iterator
         * @tparam Builder models the concept of a builder for PH
         */
        template <typename I, typename Builder>
        rd_ph_filter(I begin, I end, Builder builder) :
            ph(builder(begin, end)),
            hashes(build_filter(ph,begin,end)) {}

        template <typename X>
        auto contains(X const & x) const
        {
            return hashes[ph(x)] == hash_fn(x);
        }

        auto false_positive_rate() const
        {
            return 1.0 / std::numeric_limits<hash_type>::max();
        }

        auto false_negative_rate() const
        {
            return ph.error_rate()*(1-false_positive_rate());
        }

    private:
        PH const ph;
        std::vector<hash_type> const hashes;
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
