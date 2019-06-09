#include <utility>

using Interval = std::pair<double, double>;

template <typename T>
using Set = std::set<T>;

template <typename X, typename Y>
using Pair = std::pair<X,Y>;

Interval one() { return Interval(1,1); };
Interval zero() { return Interval(0,0); };

Interval span(const Interval& a, const Interval& b)
{
    return Interval(
        std::min(a.first, b.first),
        std::max(a.second, b.second));
};

template <typename T>
class ASet
{
public:
    bool contains(const T&);
    Interval fpr() const;
    Interval fnr() const;
    Interval tpr() const;
    Interval tnr() const;    
};

template <typename X>
class Union
{
public:
    using value_type = X;

    Union(ASet<X> const & a, ASet<X> const & b) : a(a), b(b) {};

    bool contains(X const & x) { return a.contains(x) || b.contains(x); };

    Interval fpr() const
    {
        return one() - (one() - a.fpr()) * (one() - b.fpr());
    };

    Interval fnr() const
    {
        return span(
            a.fnr() * b.fnr(),
            span(
                a.fnr()*(one()-b.fpr()),
                b.fnr()*(one()-a.fpr())));
    };

    bool operator==(Union<X> const & other)
    {
        return a == other.a && b == other.b;
    };

private:
    ASet<X> const & a;
    ASet<X> const & b;    
};

template <typename T>
class PowerSet:
    public ASet<Set<T>>
{
public:
    PowerSet(ASet<T>& a) : a(a) {};

    bool contains(const Set<T>& s) const
    {
        for (const auto& x : s)
            if (!a.contains(x)) return false;
        return true;
    };

    bool operator==(const PowerSet<T>& ps)
    {
        return a == ps.a;
    };

    Interval fpr() const
    {
        return 0;
    };

    Interval tpr() const
    {
        return 1;
    };

private:
    ASet<T>& a;
};

// what is the probability that a particular relation R <= A x B
// is a subset of A* x B* also? suppose A x B is a subset of U x U.
// then, A* and B* are also subsets of U.
//
// given a relation R in A x B, an approximate relation R* in A x B
// is just an approximate set where the members are pairs.
template <typename A, typename B>
class CartesianProduct:
    public ASet<Pair<A, B>>
{
public:
    CartesianProduct(ASet<A>& a, ASet<B>& b) : a(a), b(b) {};

    bool contains(Pair<A, B> const & p) const
    {
        return a.contains(p.first) && b.contains(p.second);
    };

    Interval fpr() const
    {
        return 0;
    };

    Interval tpr() const
    {
        return 1;
    };

    bool operator==(const CartesianProduct<A,B>& p)
    {
        return a == p.a && b = p.b;
    };

private:
    ASet<A> const & a;
    ASet<B> const & b;
};


class ApproximateRelation
{

}