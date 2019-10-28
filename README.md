# Approximate hash set (AHS) and approximate hash map (AHM): two related data types that implement the concept of the random approximate set and the random approximate map.

A random approximate set is an approximation of some objective set where false positives and false negatives occur at specified rates.


Perfect hash functions are hash functions with the additional constraint that they are one-to-one
over some specified subset. Given a set X that is to be approximated, the AHS uses a perfect hash function to map, one-to-one, the *positive* elements to a non-negative integer from 0 to N, where N is a function of the cardinality of X and the load factor of the perfect hash function.

We use a classical hash function from the universal set to some bit string of size k.
Using an positive elements perfect hash as an index into an auxilliary array of hasand then a random hash is used to map each element, whether positive or negative.
