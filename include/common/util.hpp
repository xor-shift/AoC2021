#pragma once

#include <functional>
#include <span>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "converters.hpp"

namespace AOC::Utils {

namespace SG {

enum Condition {
    Failure, Success, Any
};

template<Condition condition, typename Callable>
struct ScopeGuard {
    constexpr ScopeGuard() noexcept = delete;

    constexpr explicit ScopeGuard(Callable &&cb) noexcept(noexcept(Callable(std::forward<Callable>(cb))))
      : cb(std::forward<Callable>(cb)) {}

    constexpr explicit ScopeGuard(const Callable &cb) noexcept(noexcept(Callable(cb)))
      : cb(cb) {}

    constexpr ~ScopeGuard() noexcept {
        if (!armed) return;
        if ((condition == Condition::Failure && std::current_exception()) ||
            (condition == Condition::Success && !std::current_exception()) ||
            (condition == Condition::Any))
            std::invoke(cb);
    }

    bool armed = true;

  private:
    Callable cb;
};

template<typename Callable>
constexpr auto MakeSuccessGuard(Callable &&cb) { return ScopeGuard<Condition::Success, Callable>(std::forward<Callable>(cb)); }

template<typename Callable>
constexpr auto MakeFailureGuard(Callable &&cb) { return ScopeGuard<Condition::Failure, Callable>(std::forward<Callable>(cb)); }

template<typename Callable>
constexpr auto MakeGuard(Callable &&cb) { return ScopeGuard<Condition::Any, Callable>(std::forward<Callable>(cb)); }

}

struct MappedStringView {
    explicit MappedStringView(const std::string &filename, bool rdOnly = false);

    ~MappedStringView() noexcept;

    [[nodiscard]] char *data() noexcept { return static_cast<char *>(map); }

    [[nodiscard]] const char *data() const noexcept { return static_cast<const char *>(map); }

    [[nodiscard]] std::size_t size() const noexcept { return mapSize; }

    [[nodiscard]] explicit operator std::string_view() const noexcept { return {data(), data() + size()}; }

    [[nodiscard]] explicit operator std::span<const char>() const noexcept { return {data(), data() + size()}; }

    [[nodiscard]] explicit operator std::span<char>() {
        if (rdOnly) throw std::runtime_error(fmt::format("mapped file {} is read only", origFilename));
        return {data(), data() + size()};
    }

  private:
    const bool rdOnly;
    const std::string origFilename;

    std::size_t mapSize;
    void *map;
    int fildes;
};

extern std::vector<std::string> ReadLines(const std::string &filename);

extern std::vector<std::string_view> SplitStringView(std::string_view view);

template<typename Callable>
inline void ProcessLines(std::string_view data, std::string_view delim, Callable &&cb) {
    std::string_view::iterator start = data.begin();
    std::size_t matchCount = 0;
    std::size_t nMatches = 0;

    for (auto it = start; it != data.end(); it++) {
        if (*it == delim[matchCount]) ++matchCount;
        else matchCount = 0;

        if (matchCount >= delim.size()) {
            matchCount = 0;
            std::string_view match(start, it - matchCount);
            start = it + 1;

            std::invoke(cb, match, nMatches++);
        }
    }

    if (start != data.end()) {
        std::invoke(cb, std::string_view(start, data.end() - matchCount), nMatches);
    }
}

namespace Detail {

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

template<std::size_t n>
struct GetNLinesHelper {
    typedef TupleGen_t<std::string_view, n> tuple;

    static void Split(std::string_view view, std::string_view delimiter, tuple &t) { return Impl(view, delimiter, t); }

    static void Split(std::string_view view, char delimiter, tuple &t) { return Impl(view, delimiter, t); }

  private:
    template<std::size_t m = 0>
    static void Impl(std::string_view view, std::string_view delimiter, tuple &t) {
        std::size_t nMatches = 0;
        auto it = view.begin();

        for (; it != view.end(); it++) {
            const char c = *it;
            if (c == delimiter[nMatches]) ++nMatches;
            if (nMatches >= delimiter.size()) {
                std::get<m>(t) = std::string_view(view.begin(), it - nMatches + 1);
                break;
            }
        }

        if (it == view.end() && !view.empty()) {
            std::get<m>(t) = view;
            return;
        }

        if constexpr (m + 1 < n)
            return Impl<m + 1>(std::string_view(++it, view.end()), delimiter, t);
    }

    template<std::size_t m = 0>
    static void Impl(std::string_view view, char delimiter, tuple &t) {
        auto idx = view.find(delimiter);
        if (idx == std::string_view::npos) {
            std::get<m>(t) = view;
            return;
        } else {
            std::get<m>(t) = view.substr(0, idx);
            if constexpr (m + 1 < n) return Impl<m + 1>(view.substr(idx + 1), delimiter, t);
        }
    }
};

}

template<std::size_t n>
inline typename Detail::GetNLinesHelper<n>::tuple GetNLines(std::string_view data, auto delim) {
    typename Detail::GetNLinesHelper<n>::tuple tuple{};
    Detail::GetNLinesHelper<n>::Split(data, delim, tuple);
    return tuple;
}

inline std::vector<std::string_view> GetLines(std::string_view data, std::string_view delim) {
    std::vector<std::string_view> vec{};
    ProcessLines(data, delim, [&vec](std::string_view v, std::size_t) { vec.push_back(v); });
    return vec;
}

template<typename T>
inline auto Convert(std::string_view data, std::size_t lineNo, int intBase = 10) {
    std::string str(data);
    char *end;
    auto res = Detail::Converter<T>::Convert(str.c_str(), &end, intBase);
    if (errno) throw std::invalid_argument(fmt::format("bad value \"{}\" at line {}", data, lineNo));
    return res;
}

template<typename T, std::size_t numsPerLine, typename Callable>
requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
inline void ProcessNumbersFromLines(std::string_view data, Callable &&cb, std::string_view lineDelim = "\n", std::string_view inlineDelim = " ", int intBase = 10) {
    ProcessLines(data, lineDelim, [inlineDelim, &cb](std::string_view line, std::size_t lineNo) {
        if constexpr (numsPerLine == 1)
            std::invoke(cb, Convert<T>(line, lineNo), line, lineNo, std::size_t{0});
        else {
            ProcessLines(line, inlineDelim, [lineNo, &cb](std::string_view subsect, std::size_t no) {
                std::invoke(cb, Convert<T>(subsect, lineNo), subsect, lineNo, no);
            });
        }
    });
}

template<typename T, std::size_t numsPerLine, typename vector_t = std::conditional_t<numsPerLine == 1, std::vector<T>, std::vector<std::vector<T>>>>
requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
inline vector_t GetNumbersFromLines(std::string_view data, std::string_view lineDelim = "\n", std::string_view inlineDelim = " ", int intBase = 10) {
    vector_t vec{};
    ProcessNumbersFromLines<T, numsPerLine>(data, [&vec](T v, std::string_view, std::size_t, std::size_t) { vec.push_back(v); }, lineDelim, inlineDelim, intBase);
    return vec;
}

}
