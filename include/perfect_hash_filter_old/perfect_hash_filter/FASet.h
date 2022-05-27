#ifndef __FA_SET_H__
#define __FA_SET_H__

namespace alex::collections
{
    /*
     * A set S* is an approximate set of a finite set S if the following conditions are true:
     *  -# S is a subset of S*. This condition guarantees that no false negatives may occur.
     *  -# An element that is not a member of S is a member of S* with a probability p, denoted the *false positive rate*.
     *
     * The abstract data type of the frozen approximate set is denoted by FASet.
     */
    template <typename T>
    class FASet
    {
    public:
        typedef T element_type;
        typedef size_t cardinality;
        typedef float probability;

        /*
         * Returns an approximate set of S with false positive rate p,
         * where S is the set of unique elements in an iterable
         * collection in the range [begin, end).
         */
        template <Iter>
        virtual void make(Iter begin, Iter end, probability p) = 0;

        /*
         * Returns true if x is in S. Otherwise, returns true with probability
         * fp_rate() and false with probability 1-fp_rate().
         */
        virtual bool contains(const T& x) const = 0;

        /*
         * Returns the probability that an x not in S is in S*, i.e.,
         * the probability that x is a false positive.
         */
        virtual probability fp_rate() const = 0;

        /*
         * Returns the cardinality of approximate set S* of set S.
         */
        virtual cardinality cardinality() const = 0;

        /*
         * Returns the cardinality of set S, which is being approximated by set S*.
         */

        virtual cardinality true_cardinality() const = 0;
    };
}

#endif