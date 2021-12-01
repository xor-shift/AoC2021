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
    MappedStringView(const std::string &filename, bool rdOnly = false);

    ~MappedStringView() noexcept;

    [[nodiscard]] char *data() noexcept { return static_cast<char *>(map); }

    [[nodiscard]] const char *data() const noexcept { return static_cast<const char *>(map); }

    [[nodiscard]] std::size_t size() const noexcept { return mapSize; }

    [[nodiscard]] constexpr std::size_t max_size() const noexcept { return std::numeric_limits<std::size_t>::max(); }

    [[nodiscard]] char *begin() { return data(); }

    [[nodiscard]] char *end() { return data() + size(); }

    [[nodiscard]] const char *cbegin() const { return data(); }

    [[nodiscard]] const char *cend() const { return data() + size(); }

    [[nodiscard]] explicit operator std::string_view() const noexcept { return {cbegin(), cend()}; }

    [[nodiscard]] explicit operator std::span<const char>() const noexcept { return {cbegin(), cend()}; }

    [[nodiscard]] explicit operator std::span<char>() {
        if (rdOnly) throw std::runtime_error(fmt::format("mapped file {} is read only", origFilename));
        return {begin(), end()};
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
}

}
