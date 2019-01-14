# Perfect hash filter (PHF): an implementation of the Positive Approximate Set abstract data type

Perfect hash functions are hash functions with the additional constraint that they are one-to-one
over specified subset. A positive approximate set is an approximation of some objective set where
false positives occur at some specified probability or rate. The perfect hash filter uses a
perfect hash function to implement the abstract data type of the positive approximate set.

Note that through unions and complements, the full set-theoretic model of approximate sets, an
algebra of approximate sets over some subset of the algebra of objective sets that are the target
of approximation, where both false positives and false negatives may occur.
