#pragma once

#include <fmt/format.h>

namespace AOC::Utils {

template<typename T, typename... Ts>
void PrintShit(T &&v, Ts &&...vs) {
    constexpr std::size_t after = sizeof...(vs);
    if constexpr (after == 0) fmt::print("{}\n", std::forward<T>(v));
    else fmt::print("{}, ", std::forward<T>(v));

    if constexpr(after > 0) return PrintShit(std::forward<Ts>(vs)...);
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

}
