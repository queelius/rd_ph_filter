// An approximate set S* of a set S may be considered an
// approximate set of S where:
//     (1) An element that is not a member of S is a
//         member of S* with probability false_positive_rate().
//     (2) An element that is a member of S is not a
//         member of S* with probability false_negative_rate().
template <typename X>
class approximate_set
{
public:
    // A probability_type is a probability and
    // thus should take on a value between 0 and 1.
    typedef double probability_type;
    
    // The cardinality_type is able to represent
    // the cardinality of sets. Since an approximate
    // set may have an uncertain number of false
    // positives or true positives, the
    // cardinality_type is a real number type.
    typedef double cardinality_type;
    
    // The value_type is the type of elements
    // in the set.
    typedef X value_type;
    
    // If x is in the approximated set S, returns
    // true with probability
    //     1 - false_negative_rate().
    // Otherwise, returns true with probability
    //     false_positive_rate().
    virtual
    bool contains(const X& x) const = 0;
    
    // The cardinality of the approximate set.
    // If the approximate set has an uncertain number
    // of false positives or true positives, the
    // expected cardinality may be returned.
    virtual
    cardinality_type cardinality() const = 0;
    
    // Returns the false positive rate,
    // the probability that an element
    // not in the approximated set S
    // is in the approximate set S*.
    virtual
    probability_type false_positive_rate() const = 0;

    // Returns the false negative rate,
    // the probability that an element
    // in the approximated set S
    // is not in the approximate set S*.
    virtual
    probability_type false_negative_rate() const = 0;
};
