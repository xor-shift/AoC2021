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
inline void ProcessNumbersFromLines(std::string_view data, Callable &&cb, std::string_view lineDelim = "\n", std::string_view inlineDelim = " ", int intBase = 10) {
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

template<typename T, std::size_t numsPerLine = 1, typename vector_t = std::conditional_t<numsPerLine == 1, std::vector<T>, std::vector<std::vector<T>>>>
requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
inline vector_t GetNumbersFromLines(std::string_view data, std::string_view lineDelim = "\n", std::string_view inlineDelim = " ", int intBase = 10) {
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


template<typename K, typename V>
using umap = std::unordered_map<K, V>;

template<typename K>
using uset = std::unordered_set<K>;

namespace UseThisNamespace {
using mp_int = boost::multiprecision::gmp_int;
using Utils::umap;
using Utils::uset;
using std::stack;
using std::list;

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
