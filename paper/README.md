The *Perfect Hash Filter*
=========================

A fast and efficient implementation of the perfect hash map, which models the concept of the rate-distorted map. As special cases, we describe how it may be used to model the second-order *positive* Bernoulli set, entropy map, and more generally the rate-distorted cipher map.

Introduction
------------
The *rate-distorted perfect hash filter* models the concept of the Bernoulli
set.

In what follows, we specify the Bernoulli set abstraction then describe a
fast and efficient object type that models the concept, known as the
*rate disorted* perfect hash filter (RDPHF).

Its rate-distortion comes from two independent sources of error, false negatives
and false positives. The false negatives are a result of the rate-disorted
perfect hash function, and the false positives are a result of the filter.

Rate-distorted perfect hash filter {#sec:phf}
---------------------------------------------
The set of all bit strings of length $n$ is the Cartesian product $\{0,1\}^n$
where $|\{0,1\}^n| = 2^n$.
The *empty bit string* $\{0,1\}^0$ is denoted by $\epsilon$.
The set of all bit strings of length $0$ to $n$ is denoted by
$$
\{0,1\}^{\leq n} = \bigcup_{j=0}^{n} \{0,1\}^j
$$
where $|\{0,1\}^{\leq n}| = 2^{n+1} - 1$.
The countably infinite set of all bit strings (of any length) is denoted by $\{0,1\}^*.

The Bloom filter\cite{bf} is a well-known data structure that models the concept of a *Bernoulli set* over bit strings with a false positive rate $\varepsilon$ and a false negative rate $0$. It obtains a space complexity given approximately by
$$
    -1.44 \log_2 \varepsilon
$$
bits per element. We consider an alternative data structure, denoted the rate-disorted perfect hash filter, that also models the concept of the Bernoulli set over any 
desired type and compares favorably to the Bloom filter.

A hash function is related to countable sets $\{0,1\}^*$ and $\{0,1\}^n$ and is given by the following definition.

\begin{definition}
\label{hash_fn}
A hash function $h : T \mapsto \{0,1\}^n$ is a function such
that all bit strings of arbitrary-length are mapped (hashed) to bit strings of fixed-length $n$.
\end{definition}

For a given $x \in T$, $y = h(x) \in \{0,1\}^n$ is denoted the *hash* of $x$.
The perfect hash filter depends on a perfect hash function as given by the following definition.
\begin{definition}
A perfect hash function of the set $S$, denoted by
$$
    h_S : T \mapsto \mathbb{Z}^{+},
$$
is a hash function such that the mapping from $S$ to $\mathbb{Z}^+$ is one-to-one.
\end{definition}

The *load factor* of $h_S$ is given by the following definition.
\begin{definition}
\label{def:loadfactor}
The load factor of $h_S$ is a rational number given by
$$
\label{eq:loadfactor}
    r = \frac{m}{N},
$$
where $m = |\mathbb{S}|$, $N \geq m$, and $N$ is the maximum hash of $h_{\mathbb{S}}$.
\end{definition}
\begin{definition}
A *minimal* perfect hash function has a load factor $1$, i.e., the mapping from $\mathbb{S}$ to $\mathbb{Z}^+$ is *one-to-one* and *onto*.
\end{definition}

The PHF is a data structure given by the following definition.
\begin{definition}
The PHF is a data structure that implements\footnote{In \cref{thm:fp_rate}, we prove the PHF implements the static approximate set.} the abstract data type of the *positive approximate set* where the universe of elements is given by $\BitSet^*$.
\end{definition}


The *Perfect Hash Filter* is a product type $\textrm{PHF} = \textrm{BitMatrix} \times \textrm{PerfectHash}$ and a set of functions
\begin{enumerate}
\item $\in : \BitSet^* \times \textrm{PHF}]$
\end{enumerate}



tuple of type $[\textrm{BitMatrix}]$ that implements\footnote{In \cref{thm:fp_rate}, we prove the PHF implements the static approximate set.} the abstract data type of the *positive approximate set* where the universe of elements is given by $\BitSet^*$.


The *minimal* perfect hash filter given by the following definition.
\begin{definition}
The *minimal* perfect hash filter is a perfect hash filter with a load factor $1$, i.e., every element $x$ in the set being approximated is *perfectly hashed*.
\end{definition}
Replacing the *perfect hash function* with a *$k$-perfect hash function*, where disjoint subsets of up to $k$ elements may collide, generalizes the PHF into an approximate set where both *false positives* and *false negatives* are possible.\footnote{We may also simply approximate a subset of a set $\mathbb{S}$ to support non-zero false negative rates, but *$k$-perfect hashing* generates a more efficient solution.}






The *data type* for the perfect hash filter over the universal set $\mathbb{U}$ is defined as $hF(\mathbb{U}) = h(\mathbb{U}) \times \Bit$.
A constructor for hF with a load factor $r$,
\begin{equation}
hf : \PS{\mathbb{U}} \times \RealSet \mapsto h,
\end{equation}
is defined as
\begin{equation}
h(\mathbb{X}, r) :eqq \Tuple{n',N}
\end{equation}
where
\begin{equation}
\begin{split}
m			& :eqq |\mathbb{X}|, N :eqq \lceil m / r \rceil,k :eqq \lceil\log_2 N\rceil\\
\beta(x,n) 	& :eqq \trunc\!\left(\ro(x' \cat n'),k\right)' \mod N,\\
\mathbb{Y}[n] 	& :eqq \SetBuilder{\beta(x,n) \in \{0,\ldots,N-1\}}{x \in 
	\mathbb{X}},\\
n 			& :eqq \min{\SetBuilder{ j \in \NatSet }{ \mathbb{Y}[j] \in \PS{\NatSet} \land \Card{\mathbb{Y}[j]} = m}}.
\end{split}
\end{equation}





