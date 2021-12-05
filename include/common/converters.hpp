#pragma once

#include <cstdlib>

namespace AOC::Utils::Detail {

template<typename T, typename Enable = void>
struct Converter { static T Convert(const char *str, char **end, int intBase); };

template<typename T>
struct Converter<T, std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T> && (sizeof(T) <= sizeof(long)), void>> {
    static T Convert(const char *str, char **end, int intBase) { return static_cast<T>(std::strtol(str, end, intBase)); }
};

template<typename T>
struct Converter<T, std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) <= sizeof(unsigned long)), void>> {
    static T Convert(const char *str, char **end, int intBase) { return static_cast<T>(std::strtoul(str, end, intBase)); }
};

template<typename T>
struct Converter<T, std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T> && (sizeof(T) > sizeof(long)), void>> {
    static T Convert(const char *str, char **end, int intBase) { return static_cast<T>(std::strtoll(str, end, intBase)); }
};

template<typename T>
struct Converter<T, std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) > sizeof(unsigned long)), void>> {
    static T Convert(const char *str, char **end, int intBase) { return static_cast<T>(std::strtoull(str, end, intBase)); }
};

template<typename T>
struct Converter<T, std::enable_if_t<std::is_same_v<T, float>, void>> { static T Convert(const char *str, char **end, int intBase) { return std::strtof(str, end); }};

template<typename T>
struct Converter<T, std::enable_if_t<std::is_same_v<T, double>, void>> { static T Convert(const char *str, char **end, int intBase) { return std::strtod(str, end); }};

template<typename T>
struct Converter<T, std::enable_if_t<std::is_same_v<T, long double>, void>> { static T Convert(const char *str, char **end, int intBase) { return std::strtold(str, end); }};

}

namespace AOC::Utils {

template<typename T>
inline auto Convert(std::string_view data, int intBase = 10) {
    std::string str(data);
    char *end;
    auto res = Detail::Converter<T>::Convert(str.c_str(), &end, intBase);
    if (errno) throw std::invalid_argument(fmt::format("bad value \"{}\" for conversion", data));
    return res;
}

}
