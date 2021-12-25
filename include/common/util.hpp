#pragma once

#include <functional>
#include <span>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/gmp.hpp>
#include <fmt/format.h>

#include "converters.hpp"
#include "io.hpp"
#include "scope_guard.hpp"
#include "tuple.hpp"

namespace AOC::Utils {

namespace Detail {

template<typename Callable>
inline void ProcessLines(std::string_view data, auto delim, Callable &&cb, bool ignoreEmpty) {
    std::string_view::iterator start = data.begin();
    std::size_t nMatches = 0;

    if constexpr (std::is_same_v<std::string_view, decltype(delim)>) {
        std::size_t matchCount = 0;
        for (auto it = start; it != data.end(); it++) {
            if (*it == delim[matchCount]) ++matchCount;
            else matchCount = 0;

            if (matchCount >= delim.size()) {
                std::string_view match(start, it - matchCount + 1);
                matchCount = 0;
                start = it + 1;

                if (!match.empty() || !ignoreEmpty)
                    std::invoke(cb, match, nMatches++);
            }
        }
    } else {
        for (auto it = start; it != data.end(); it++) {
            if (*it == delim) {
                std::string_view match(start, it);
                start = it + 1;

                if (!match.empty() || !ignoreEmpty)
                    std::invoke(cb, match, nMatches++);
            }
        }
    }

    if (start != data.end()) {
        std::invoke(cb, std::string_view(start, data.end()), nMatches);
    }
}

}

template<typename Callable>
inline void ProcessLines(std::string_view data, std::string_view delim, Callable &&cb, bool ignoreEmpty = true) {
    return Detail::ProcessLines<Callable>(data, delim, std::forward<Callable>(cb), ignoreEmpty);
}

template<typename Callable>
inline void ProcessLines(std::string_view data, char delim, Callable &&cb, bool ignoreEmpty = true) {
    return Detail::ProcessLines<Callable>(data, delim, std::forward<Callable>(cb), ignoreEmpty);
}

inline std::vector<std::string_view> GetLines(std::string_view data, std::string_view delim, bool ignoreEmpty = true) {
    std::vector<std::string_view> vec{};
    ProcessLines(data, delim, [&vec](std::string_view v, std::size_t) { vec.push_back(v); }, ignoreEmpty);
    return vec;
}

inline std::vector<std::string_view> GetLines(std::string_view data, char delim, bool ignoreEmpty = true) {
    std::vector<std::string_view> vec{};
    ProcessLines(data, delim, [&vec](std::string_view v, std::size_t) { vec.push_back(v); }, ignoreEmpty);
    return vec;
}

template<typename T, std::size_t numsPerLine, typename Callable>
requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
inline void ProcessNumbersFromLines(std::string_view data, Callable &&cb, std::string_view lineDelim = "\n",
                                    std::string_view inlineDelim = " ", int intBase = 10) {
    ProcessLines(data, lineDelim, [inlineDelim, &cb](std::string_view line, std::size_t lineNo) {
        if constexpr (numsPerLine == 1)
            std::invoke(cb, Convert<T>(line), line, lineNo, std::size_t{0});
        else {
            ProcessLines(line, inlineDelim, [lineNo, &cb](std::string_view subsect, std::size_t no) {
                std::invoke(cb, Convert<T>(subsect), subsect, lineNo, no);
            });
        }
    });
}

template<typename T, std::size_t numsPerLine = 1, typename vector_t = std::conditional_t<
  numsPerLine == 1, std::vector<T>, std::vector<std::vector<T>>>>
requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
inline vector_t
GetNumbersFromLines(std::string_view data, std::string_view lineDelim = "\n", std::string_view inlineDelim = " ",
                    int intBase = 10) {
    vector_t vec{};
    ProcessNumbersFromLines<T, numsPerLine>(data, [&vec](T v, std::string_view, std::size_t r, std::size_t c) {
        if constexpr (numsPerLine == 1) {
            vec.push_back(v);
        } else {
            if (!c) {
                vec.push_back({});
            }

            vec.back()->push_back(v);
        }
    }, lineDelim, inlineDelim, intBase);
    return vec;
}

template<bool use_bools = false>
auto hex_to_binary(std::string_view hex) {
    static const std::unordered_map<char, std::string> conv{
      {'0', "0000"},
      {'1', "0001"},
      {'2', "0010"},
      {'3', "0011"},
      {'4', "0100"},
      {'5', "0101"},
      {'6', "0110"},
      {'7', "0111"},
      {'8', "1000"},
      {'9', "1001"},
      {'A', "1010"},
      {'B', "1011"},
      {'C', "1100"},
      {'D', "1101"},
      {'E', "1110"},
      {'F', "1111"},
    };

    std::vector<std::conditional_t<use_bools, bool, uint8_t>> ret{};
    ret.reserve(hex.size());
    for (char c: hex) {
        auto corresp = conv.at(c);

        for (char b: corresp) {
            ret.push_back(b == '1');
        }
    }

    return ret;
}

template<typename T, std::size_t n>
struct ArrayHasher {
    std::size_t operator()(const std::array<T, n> &arr) const {
        std::string sum;
        for (std::size_t i = 0; i < n; i++) {
            sum += fmt::to_string(arr[i]);
        }

        return std::hash<std::string>{}(sum);
    }
};

namespace Detail {

static inline std::size_t hash_combine(std::size_t lhs, std::size_t rhs) {
    constexpr std::size_t k_magic_phi =
      sizeof(std::size_t) == sizeof(uint64_t) ? 0x9e3779b97f4a7c15ull : 0x9e3779b9ul;
    constexpr std::size_t k_magic_prime =
      sizeof(std::size_t) == sizeof(uint64_t) ? 0x9e3779b97f4a7c55ull : 0x9e3779b1ul;

    return lhs ^ rhs + k_magic_prime + (lhs << 6) + (lhs >> 2);
}

template<typename T, typename ...Ts>
static inline std::size_t multi_hash(const T &v, const Ts &...vs) {
    std::size_t lhs = std::hash<T>{}(v);

    if constexpr (sizeof...(Ts) > 0)
        return hash_combine(lhs, multi_hash(vs...));
    else
        return lhs;
}

}

template<typename... Ts>
struct TupleHasher {
    template<std::size_t i = 0>
    std::size_t operator()(const std::tuple<Ts...> &tuple) const {
        const auto &lhs_elem = std::get<i>(tuple);

        std::size_t lhs = std::hash<std::decay_t<decltype(lhs_elem)>>{}(lhs_elem);

        if constexpr (i + 1 == sizeof...(Ts)) return lhs;
        else return Detail::hash_combine(lhs, operator()<i + 1>(tuple));
    }

  private:
};

template<typename Ret, typename... Args>
class MemoizationWrapper {
    using ArgsTuple = std::tuple<Args...>;
    using MemoPair = std::pair<ArgsTuple, Ret>;
    using ListType = std::list<MemoPair>;
    using NewMapType = std::unordered_map<ArgsTuple, typename ListType::iterator, TupleHasher<Args...>>;

    using MapType = std::unordered_map<ArgsTuple, Ret, TupleHasher<Args...>>;

  public:
    MemoizationWrapper() noexcept = default;

    MemoizationWrapper(std::size_t lru_size)
      : m_max_saved_calls(lru_size) {}

    Ret operator()(Args... args) {
        return call(std::move(args)...);
    }

    Ret call_old(Args... args) {
        ArgsTuple args_tuple{args...};

        auto it = m_memoization.find(args_tuple);
        if (it != m_memoization.end()) {
            ++m_memoized_calls;
            return it->second;
        }

        ++m_computed_calls;

        Ret ret = impl(args...);

        m_memoization.insert({args_tuple, ret});
        return ret;
    }

    Ret call(Args... args) {
        ArgsTuple args_tuple{args...};

        auto it = m_new_memoization.find(args_tuple);
        if (it != m_new_memoization.end()) {
            ++m_memoized_calls;

            auto list_it = it->second;
            const auto res = list_it->second;
            it->second = m_list.emplace(m_list.end(), std::move(*list_it));
            m_list.erase(list_it);

            return res;
        }

        ++m_computed_calls;

        Ret ret = impl(args...);

        auto list_pair = typename ListType::value_type{args_tuple, ret};
        auto list_it = m_list.emplace(m_list.end(), std::move(list_pair));
        m_new_memoization.insert({args_tuple, list_it});

        while (m_list.size() >= m_max_saved_calls) {
            m_new_memoization.erase(m_list.front().first);
            m_list.pop_front();
        }

        return ret;
    }

    long double memoized_ratio() {
        const auto successful = static_cast<long double>(m_memoized_calls);
        const auto computed = static_cast<long double>(m_computed_calls);

        return (successful) / (computed + successful);
    }

    uint64_t m_memoized_calls = 0;
    uint64_t m_computed_calls = 0;

  protected:
    const std::size_t m_max_saved_calls = 512ull * 1024ull;
    ListType m_list{};
    NewMapType m_new_memoization{};

    MapType m_memoization{};

    virtual Ret impl(Args... args) = 0;
};

namespace UseThisNamespace {
template<typename K, typename V>
using umap = std::unordered_map<K, V>;

template<typename... Args>
using uset = std::unordered_set<Args...>;

template<typename V>
using vec = std::vector<V>;

template<typename V, std::size_t n>
using arr = std::array<V, n>;

template<typename V>
using opt = std::optional<V>;

using std::stack;
using std::list;
using std::pair;

using mp_int = boost::multiprecision::cpp_int;

using namespace std::ranges;
using namespace Utils;
}

}

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
