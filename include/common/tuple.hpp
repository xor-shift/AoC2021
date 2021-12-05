#pragma once

#include <tuple>
#include <vector>

#include "converters.hpp"

namespace AOC::Utils::Detail {

template<typename LHS, typename RHS>
struct CombineTuples {};

template<typename... LHSTs, typename... RHSTs>
struct CombineTuples<std::tuple<LHSTs...>, std::tuple<RHSTs...>> { typedef std::tuple<LHSTs..., RHSTs...> type; };

template<typename T, std::size_t n>
struct TupleGen {
    typedef typename TupleGen<T, n / 2>::type left;
    typedef typename TupleGen<T, n / 2 + n % 2>::type right;
    typedef typename CombineTuples<left, right>::type type;
};

template<typename T>
struct TupleGen<T, 0> { typedef std::tuple<> type; };

template<typename T>
struct TupleGen<T, 1> { typedef std::tuple<T> type; };

template<typename T, std::size_t n>
using TupleGen_t = typename TupleGen<T, n>::type;

template<typename T, std::size_t size, std::size_t n = 0>
std::size_t VecToTuple(const std::vector<T> &vec, Detail::TupleGen_t<T, size> &tuple) {
    if (vec.size() < n) return n;
    std::get<n>(tuple) = vec[n];
    if constexpr (n + 1 < size) return VecToTuple<T, size, n + 1>(vec, tuple);
    else return size;
}

template<typename UnaryOp, typename T, typename... Ts>
struct TransformTupleType {
    using type = std::conditional_t<sizeof...(Ts) == 1,
      std::tuple<std::invoke_result_t<UnaryOp, const T &>, std::invoke_result_t<UnaryOp, const Ts &...>>,
      typename CombineTuples<
        std::tuple<std::invoke_result_t<UnaryOp, const T &>>,
        typename TransformTupleType<UnaryOp, Ts...>::type
      >::type
    >;
};


template<typename UnaryOp, std::size_t i, typename... Ts>
inline void TransformTuple(auto &out, const std::tuple<Ts...> &tuple, UnaryOp &&op) {
    std::get<i>(out) = std::invoke(op, std::get<i>(tuple));
    if constexpr (i + 1 < sizeof...(Ts)) return TransformTuple<UnaryOp, i + 1, Ts...>(out, tuple, std::forward<UnaryOp>(op));
}

}

namespace AOC::Utils {

template<std::size_t n, typename T>
inline Detail::TupleGen_t<T, n> VecToTuple(const std::vector<T> &vec) {
    Detail::TupleGen_t<std::string_view, n> tuple{};
    Detail::VecToTuple<T, n>(vec, tuple);
    return tuple;
}

template<typename UnaryOp, typename T, typename... Ts>
inline auto Map(const std::tuple<T, Ts...> &tuple, UnaryOp &&op) {
    using U = std::invoke_result_t<UnaryOp, const T &>;
    Detail::TupleGen_t<U, sizeof...(Ts) + 1> ret;
    Detail::TransformTuple<UnaryOp, 0, Ts...>(ret, tuple, std::forward<UnaryOp>(op));
    return ret;
}

template<typename T, typename U>
std::pair<T, U> ToPair(const std::tuple<T, U> &t) { return {std::get<0>(t), std::get<1>(t)}; }

template<typename T>
std::pair<T, T> ToPair(const std::vector<T> &v) {
    std::pair<T, T> p{};
    if (v.size() > 0) p.first = v[0];
    if (v.size() > 1) p.second = v[1];
    return p;
}

template<typename UnaryOp, typename T, typename U>
inline auto Map(const std::pair<T, U> &p, UnaryOp &&op) { return std::make_pair(std::invoke(op, p.first), std::invoke(op, p.second)); }

template<typename To, typename T, typename U>
inline std::pair<To, To> Convert(const std::pair<T, U> &v, int intBase = 10) {
    return {Convert<To>(v.first, intBase), Convert<To>(v.second, intBase)};
}

}

namespace std {

template<>
struct hash<std::pair<uint32_t, uint32_t>> {
    std::size_t operator()(const std::pair<uint32_t, uint32_t> &p) const {
        return std::hash<uint64_t>{}((static_cast<uint64_t>(p.second) << 32) | p.first);
    }
};

}
