#include <random>
#include <utility>
#include <algorithm>
#include <set>
#include <map>
#include <variant>


namespace alex::random_approximate_set
{
using interval = std::pair<double, double>;

interval one() { return interval(1,1); };
interval zero() { return interval(0,0); };

interval span(const interval& a, const interval& b)
{
    return interval(
        std::min(a.first, b.first),
        std::max(a.second, b.second));
};


template <
    typename T,                     // sets are elements of type T
    template <typename> typename L, // L models an approximate set type
    template <typename> typename R, // R models an approximate set type
                                    // Note: L and R should be parameterized by type T
                                    //       for union to make sense. See DisjointUnion
                                    //       to relax this constraint.
    typename I = interval           // I models an interval type
>
class Union
{
public:
    using value_type = T;
    using interval_type = I;

    Union(L const & a, R const & b) : a(a), b(b) {};

    bool contains(T const & x)
    {
        return a.contains(x) || b.contains(x);
    };

    I fpr() const
    {
        return one() - (one() - a.fpr()) * (one() - b.fpr());
    };

    I fnr() const
    {
        return span(
            a.fnr() * b.fnr(),
            span(
                a.fnr()*(one()-b.fpr()),
                b.fnr()*(one()-a.fpr())));
    };

    bool operator==(Union<T,L,R,I> const & other)
    {
        return a == other.a && b == other.b;
    };

private:
    L<T> a;
    R<T> b;    
};

template <
    typename T,
    template <typename> typename L,
    template <typename> typename R,
    typename I = interval
>
class Intersection
{
public:
    using value_type = T;
    using interval_type = I;

    Intersection(L const & a, R const & b) : a(a), b(b) {};

    bool contains(T const & x)
    {
        return a.contains(x) && b.contains(x);
    };

    I fpr() const
    {
        return one() - (one() - a.fpr()) * (one() - b.fpr());
    };

    I fnr() const
    {
        return span(
            a.fnr() * b.fnr(),
            span(
                a.fnr()*(one()-b.fpr()),
                b.fnr()*(one()-a.fpr())));
    };

    bool operator==(Intersection<T,L,R,I> const & other)
    {
        return a == other.a && b == other.b;
    };

private:
    L<T> a;
    R<T> b;    
};


template <
    typename L,
    typename R,
    typename I = interval
>
class DisjointUnion
{
public:
    using left_value_type = typename L::value_type;
    using right_value_type = typename R::value_type;
    using value_type = std::variant<
        left_value_type,
        right_value_type>;

    DisjointUnion(L a, R b) : a(a), b(b) {};

    bool contains(value_type const & x)
    {
        return x.index() == 0 ?
            a.contains(x) : b.contains(x);
    };

    I fpr() const
    {
        return span(left.fpr(), right.fpr());
    };

    I fnr() const
    {
        return span(left.fnr(), right.fnr());;
    };

    bool operator==(DisjointUnion<L,R,I> const & other)
    {
        return left == other.a && b == other.b;
    };

    auto cardinality() const
    {
        return cardinality(left) + cardinality(right);
    };

private:
    L left;
    R right;    
};


template <typename ASet>
class PowerSet
{
public:
    PowerSet(ASet a) : a(a) {};

    using interval_type = typename ASet::interval_type;
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

    auto cardinality() const
    {
        return std::pow(2., a.cardinality());
    };

    interval_type fpr() const
    {
        return {0,0};
    };

    interval_type fnr() const
    {
        return {0,0};
    };

private:
    ASet a;
};

template <
    typename L,
    typename R
>
class CartesianProduct
{
public:
    CartesianProduct(L const & a, R const & b) : a(a), b(b) {};

    using left_value_type = typename Left::value_type;
    using right_value_type = typename Right::value_type;
    using value_type = std::pair<left_value_type, right_value_type>;
    using interval_type = typename Left::interval_type;

    bool contains(value_type const & p) const
    {
        return a.contains(p.first) && b.contains(p.second);
    };

    interval_type fpr() const
    {
        return 0;
    };

    interval_type tpr() const
    {
        return 1;
    };

    bool operator==(const CartesianProduct<L,R>& other)
    {
        return a == other.a && b = other.b;
    };

    auto cardinality() const
    {
        return cardinality(a) * cardinality(b);
    };

    auto left_project() const
    {
        return p.left;
    };

    auto right_project() const
    {
        return p.right;
    };

    auto converse() const
    {
        return CartesianProduct<R,L>(b, a);
    };

private:
    L a;
    R b;
};

template <typename T>
struct EmptySet
{
    bool contains(T const &) const { return false; };
};

template <typename T>
struct UniversalSet
{
    bool contains(T const &) const { return true; };
};


template <
    typename T,
    template <typename> typename L,
    template <typename> typename R
>
auto make_union(L<T> left, R<T> right)
{
    return Union<T,L,R>(left, right);
}

template <
    typename T,
    typename R
>
auto
make_union(EmptySet<T> left, R right)
{
    return right;
};

template <
    typename L, // L models Approximate Set concept
    typename T 
>
auto
make_union(L left, EmptySet<T> right)
{
    return left;
};

template <
    typename L, // L models Approximate Set concept
    typename T 
>
auto
make_union(L left, UniversalSet<T> right)
{
    return right;
};

template <
    typename L, // L models Approximate Set concept
    typename T 
>
auto
make_union(UniversalSet<T> left, R right)
{
    return left;
};

template <
    typename L,
    typename R
>
auto
make_disjoint_union(L left, R right)
{
    return DisjointUnion<L,R>(left,right);
}

template <
    typename T,
    typename R
>
auto
make_disjoint_union(EmptySet<T> left, R right)
{
    return right;
};

template <
    typename Left,
    typename T
>
auto
make_disjoint_union(Left left, EmptySet<T> right)
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

};