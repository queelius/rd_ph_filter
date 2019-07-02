#include <random>
#include <utility>
#include <algorithm>
#include <set>
#include <map>
#include <variant>


// do we want to call this an algebraic data type? a data type is a set an elements
// of the set are its values.
// 

// what is the probability that a particular relation R <= A x B
// is a subset of A* x B* also? suppose A x B is a subset of U x U.
// then, A* and B* are also subsets of U.
//
// given a relation R in A x B, an approximate relation R* in A x B
// is just an approximate set where the members are pairs.

using interval = std::pair<double, double>;

interval one() { return interval(1,1); };
interval zero() { return interval(0,0); };

interval span(const interval& a, const interval& b)
{
    return interval(
        std::min(a.first, b.first),
        std::max(a.second, b.second));
};

template <typename T>
class ASetRandom
{
public:
    template <typename I> // I models forward iterator
    ASetRandom(I begin, I end, double fpr, double tpr) :
        s(begin, end),
        fpr(fpr),
        tpr(tpr) {};

    interval fpr() const
    {
        return {fpr,fpr};
    };

    interval fpr() const
    {
        return {fpr,fpr};
    };

    bool contains(const T& x) const
    {
        if (p.count(x) == 0)
        {
            std::random_device r;
            std::default_random_engine g(r());

            if (s.count(x) == 1)
            {
                std::bernoulli_distribution test(fnr.first);
                p[x] = !test(g);
            }
            else
            {
                std::bernoulli_distribution test(fpr.first);
                p[x] = !test(g);
            }
        }
        return p[x];
    };

private:
    std::set<T> s;
    mutable std::map<T, bool> p;
};

template <
    typename T,
    template <typename> typename L,
    template <typename> typename R
>
class Union
{
public:
    using value_type = T;

    Union(Left a, Right b) : a(a), b(b) {};

    bool contains(T const & x) { return a.contains(x) || b.contains(x); };

    interval fpr() const
    {
        return one() - (one() - a.fpr()) * (one() - b.fpr());
    };

    interval fnr() const
    {
        return span(
            a.fnr() * b.fnr(),
            span(
                a.fnr()*(one()-b.fpr()),
                b.fnr()*(one()-a.fpr())));
    };

    bool operator==(Union<T,Left,Right> const & other)
    {
        return a == other.a && b == other.b;
    };

private:
    Left<T> a;
    Right<T> b;    
};

template <
    typename T,
    template <typename> typename Left,
    template <typename> typename Right
>
class Intersection
{
public:
    using value_type = T;

    Intersection(Left a, Right b) : a(a), b(b) {};

    bool contains(T const & x) { return a.contains(x) && b.contains(x); };

    interval fpr() const
    {
        return one() - (one() - a.fpr()) * (one() - b.fpr());
    };

    interval fnr() const
    {
        return span(
            a.fnr() * b.fnr(),
            span(
                a.fnr()*(one()-b.fpr()),
                b.fnr()*(one()-a.fpr())));
    };

    bool operator==(Intersection<T,Left,Right> const & other)
    {
        return a == other.a && b == other.b;
    };

private:
    Left<T> a;
    Right<T> b;    
};


template <
    typename Left,
    typename Right
>
class DiscrimantedUnion
{
public:
    using left_value_type = typename Left::value_type;
    using right_value_type = typename Left::value_type;
    using value_type = std::variant<
        left_value_type,
        right_value_type>;

    DiscrimantedUnion(Left a, Right b) : a(a), b(b) {};

    bool contains(value_type const & x)
    {
        if (x is same as left_value_type)
            return a.contains(x) || b.contains(x);
    };

    interval fpr() const
    {
        return one() - (one() - a.fpr()) * (one() - b.fpr());
    };

    interval fnr() const
    {
        return span(
            a.fnr() * b.fnr(),
            span(
                a.fnr()*(one()-b.fpr()),
                b.fnr()*(one()-a.fpr())));
    };

    bool operator==(DiscrimantedUnion<Left,Right> const & other)
    {
        return left == other.a && b == other.b;
    };

private:
    Left left;
    Right right;    
};


template <typename ASet>
class PowerSet
{
public:
    PowerSet(ASet a) : a(a) {};

    using value_type = std::set<typename ASet::value_type>;

    bool contains(value_type const & s) const
    {
        for (const auto& x : s)
            if (!a.contains(x)) return false;
        return true;
    };

    bool operator==(const PowerSet<ASet>& ps)
    {
        return a == ps.a;
    };

    interval fpr() const
    {
        return 0;
    };

    interval tpr() const
    {
        return 1;
    };

private:
    ASet a;
};

template <
    typename Left,
    typename Right
>
class CartesianProduct
{
public:
    CartesianProduct(Left a, Right b) : a(a), b(b) {};

    using left_value_type = typename Left::value_type;
    using right_value_type = typename Right::value_type;
    using value_type = std::pair<left_value_type, right_value_type>;

    bool contains(value_type const & p) const
    {
        return a.contains(p.first) && b.contains(p.second);
    };

    interval fpr() const
    {
        return 0;
    };

    interval tpr() const
    {
        return 1;
    };

    bool operator==(const CartesianProduct<Left,Right>& other)
    {
        return a == other.a && b = other.b;
    };

    double cardinality() const
    {
        return cardinality(p.left) * cardinality(p.right);
    };

    auto left_project() const
    {
        return p.left;
    };

    auto right_project() const
    {
        return p.right;
    };

    auto flip() const
    {
        return CartesianProduct<Right,Left>(b, a);
    };

private:
    Left a;
    Right b;
};

template <typename T>
struct EmptySet
{
    bool contains(const T&) const { return false; };
};

template <
    typename T,
    template <typename> typename Left,
    template <typename> typename Right
>
auto make_union(Left<T> left, Right<T> right)
{
    return Union<T,Left,Right>(left, right);
}

template <
    typename Left, // Left models Approximate Set concept
    typename T 
>
auto
make_union(Left left, EmptySet<T> right)
{
    return left;
}

template <
    typename T,
    typename Right
>
auto
make_union(EmptySet<T> left, Right right)
{
    return right;
};

template <
    typename Left,
    typename Right
>
auto
make_discriminated_union(Left left, Right right)
{
    return DiscriminatedUnion<Left,Right>(left,right);
}

template <
    typename T,
    typename Right
>
auto
make_discriminated_union(EmptySet<T> left, Right right)
{
    return right;
};

template <
    typename Left,
    typename T
>
auto
make_discriminated_union(Left left, EmptySet<T> right)
{
    return left;
};

template <
    typename T,
    typename Right
>
auto
make_intersection(EmptySet<T> left, Right right)
{
    return left;
}

template <
    typename T,
    typename Right
>
auto
make_intersection(EmptySet<T> left, Right right)
{
    return left;
};

template <
    typename T,
    typename Right
>
auto
make_cartesian_product(EmptySet<T> left, Right right)
{
    return left;
};

template <
    typename Left,
    typename T
>
auto
make_cartesian_product(Left left, EmptySet<T> right)
{
    return right;
};

template <
    typename Left,
    typename Right
>
auto
make_cartesian_product(Left left, Right right)
{
    return CartesianProduct<Left,Right>(left,right);
};




using HiddenKeyword = std::string;
using HiddenTag = std::string;
using HiddenBiword = std::pair<HiddenKeyword, HiddenKeyword>;
using HiddenBiwordQuery = std::variant<
    HiddenKeyword,
    HiddenTag,
    HiddenBiword
>;

// PASet<HiddenBiwordQuery>
//
// contains :: (PASet<HiddenBiwordQuery>, HiddenBiwordQuery) -> bool
//
// Let a :: PASet<HiddenBiwordQuery> =
//     generate


// What if we union sets of different types? This should be possible. It should result
// in a sum type!