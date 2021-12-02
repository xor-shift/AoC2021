#pragma once

#include <functional>
#include <span>
#include <string>
#include <vector>

#include <fmt/format.h>

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
void ProcessLines(std::string_view data, std::string_view delim, Callable &&cb) {
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

template<typename Conv, typename ...Args>
auto ConvertUsing(std::string_view data, std::size_t lineNo, Conv &&conv, Args ...args) {
    std::string str(data);
    char *end;
    auto res = std::strtoull(str.c_str(), &end, args...);
    if (errno) throw std::invalid_argument(fmt::format("bad value \"{}\" at line {}", data, lineNo));
    return res;
}


template<typename T, std::size_t numsPerLine, typename Callable>
requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
void ProcessNumbersFromLines(std::string_view data, Callable &&cb, std::string_view lineDelim = "\n", std::string_view inlineDelim = " ", int intBase = 10) {
    auto ConvertOne = [intBase](std::string_view data, std::size_t lineNo) {
        std::string conv(data);
        if constexpr (std::is_integral_v<T>) {
            if constexpr (std::is_unsigned_v<T>)
                return static_cast<T>(ConvertUsing(data, lineNo, [](const char *s, char **e, int b) { return std::strtoull(s, e, b); }, 10));
            else
                return static_cast<T>(ConvertUsing(data, lineNo, [](const char *s, char **e, int b) { return std::strtoll(s, e, b); }, 10));
        } else if constexpr (std::is_floating_point_v<T>) {
            return static_cast<T>(ConvertUsing(data, lineNo, [](const char *s, char **e) { return std::strtof(s, e); }));
        } else {
            static_assert(std::is_same_v<T, T>);
        }
    };

    ProcessLines(data, lineDelim, [inlineDelim, ConvertOne, &cb](std::string_view line, std::size_t lineNo) {
        if constexpr (numsPerLine == 1)
            std::invoke(cb, ConvertOne(line, lineNo), line, lineNo, std::size_t{0});
        else {
            ProcessLines(line, inlineDelim, [lineNo, ConvertOne, &cb](std::string_view subsect, std::size_t no) {
                std::invoke(cb, ConvertOne(subsect, lineNo), subsect, lineNo, no);
            });
        }
    });
}

template<typename T, std::size_t numsPerLine, typename vector_t = std::conditional_t<numsPerLine == 1, std::vector<T>, std::vector<std::vector<T>>>>
requires (std::is_integral_v<T> || std::is_floating_point_v<T>)
vector_t GetNumbersFromLines(std::string_view data, std::string_view lineDelim = "\n", std::string_view inlineDelim = " ", int intBase = 10) {
    vector_t vec{};
    ProcessNumbersFromLines<T, numsPerLine>(data, [&vec](T v, std::string_view, std::size_t, std::size_t) { vec.push_back(v); }, lineDelim, inlineDelim, intBase);
    return vec;
}

}
