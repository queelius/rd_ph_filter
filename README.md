Rate-distorted perfect hash filter
==================================

The library `rd_phfilter`, which standards for rate-distorted perfect hash filter, models the concept of
a bernoulli set. It is a very practical implementation of the concept.

The rate-distortion occurs in two independent ways, yielding two different types of rate-distortion.
The first distortion is given by perfectly hashing each element of the objective set, and then
storing its hash (not the perfect hash, but a standard hash) at the index the perfect hash
function assigns to it. There is a small chance that a random element not in the objective
set will hash to the same value. This causes a type of rate distortion denoted the
false positive rate.

The second kind of rate distortion occurs as a function of the rate-distortion in the
perfect hash function. If the perfect hash function fails to perfectly hash a particular
element of the objective set, then it will collide with another element in the objective
set. In this case, it still has a small chance of testing positive for membership in the
objective set if it, with a small probability, hashes (not the perfect hash) to the same
value. This type of rate distortion is known as the false negative rate.

There is an opportunity to store the hashes in k bits, say 9 bits, with something like a
*packed matrix*, in which case we could reduce the space requirements by some amount at
the cost of a larger false positive rate and time complexity. We did not consider it
worth the extra cost to implement for this particular data structure. However, it may
be modeled with the `disjoint_hash_map` data structure when paired with an appropriate
coder. The `disjoint_hash_map` models the concept of a Bernoulli map (a type of
rate-distorted map).
