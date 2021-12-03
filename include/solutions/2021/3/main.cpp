#include "main.hpp"

namespace AOC::Y2021::D3 {

void FirstSolution(std::string_view data) {
    constexpr bool test = false;
    constexpr std::size_t k = test ? 5 : 12;
    //constexpr std::size_t k = 5;
    using freq_t = std::array<std::pair<std::size_t, std::size_t>, k>;
    std::array<std::pair<std::size_t, std::size_t>, k> frequencies{};

    Utils::ProcessLines(data, "\n", [&frequencies](std::string_view line, std::size_t n) mutable {
        for (std::size_t i = 0; i < k; i++) {
            auto &s = (line[i] == '1') ? frequencies[i].second : frequencies[i].first;
            ++s;
        }
    });

    //for (auto[a, b]: frequencies) fmt::print("{}, {}\n", a, b);

    std::size_t g = 0, e = 0;
    for (std::size_t i = 0; i < k; i++) {
        auto shift = k - i - 1;
        std::size_t bit = frequencies[i].second > frequencies[i].first;
        g |= (bit << shift);
        e |= ((!bit) << shift);
    }
    fmt::print("{} * {} = {}\n", g, e, g * e);

    auto lines = Utils::GetLines(data, "\n");
    std::vector<std::string_view> passedo{lines};
    std::vector<std::string_view> passedco{lines};

    auto getFrequencies = [k](std::vector<std::string_view> &v) {
        std::array<std::pair<std::size_t, std::size_t>, k> frequencies{};
        for (auto l: v) {
            for (std::size_t i = 0; i < k; i++) {
                auto &s = (l[i] == '1') ? frequencies[i].second : frequencies[i].first;
                ++s;
            }
        }
        return frequencies;
    };

    std::string_view r0{}, r1{};

    auto remove = [&](freq_t &f, std::vector<std::string_view> &v, std::size_t pos, bool criteria, bool asd) {
        std::vector<std::size_t> to{};
        for (std::size_t i = 0; i < v.size(); i++) if ((v[i][pos] == '1') != criteria) to.push_back(i);
        for (auto it = to.crbegin(); it != to.crend(); it++)
            v.erase(v.begin() + *it);

        //for (auto l : v) fmt::print("{}, ", l);
        //fmt::print("\n");

        if (v.size() == 1) {
            fmt::print("{}\n", v[0]);
            if (asd) r1 = v[0];
            else r0 = v[0];
        }
    };

    for (std::size_t i = 0; i < k; i++) {
        auto f = getFrequencies(passedo);
        remove(f, passedo, i, f[i].second >= f[i].first, false);
    }

    for (std::size_t i = 0; i < k; i++) {
        auto f = getFrequencies(passedco);
        remove(f, passedco, i, f[i].second < f[i].first, true);
    }

    fmt::print("{}\n", Utils::Convert<unsigned>(r0, 0, 2) * Utils::Convert<unsigned>(r1, 0, 2));
}

void Refactor(std::string_view data) {
    using lines_t = std::vector<std::string_view>;
    auto lines = Utils::GetLines(data, "\n");
    auto bitWidth = lines[0].size();

    auto Freq = [](const lines_t &ls, std::size_t idx, auto &&cmp) {
        std::size_t f[2]{0, 0};
        for (auto l: ls) ++f[l[idx] == '1'];
        return cmp(f[0], f[1]);
    };

    auto Remove = [Freq](lines_t &vec, std::size_t atIndex, auto &&predicate, std::size_t &out) {
        bool wantOne = Freq(vec, atIndex, predicate);
        vec.erase(std::remove_if(vec.begin(), vec.end(), [wantOne, i = atIndex](std::string_view v) { return (v[i] == '1') != wantOne; }), vec.end());
        if (vec.size() == 1) out = Utils::Convert<std::size_t>(vec[0], 0, 2);
    };

    std::size_t part1 = 0, part2o2 = 0, part2co2 = 0;
    auto o2 = lines, co2 = lines;
    for (std::size_t i = 0; i < bitWidth; i++) {
        std::size_t b = Freq(lines, i, std::less{});
        part1 |= b << (bitWidth - i - 1);

        Remove(co2, i, std::greater{}, part2o2);
        Remove(o2, i, std::less_equal{}, part2co2);
    }
    fmt::print("part1: {}\n", part1 * (~part1 & ((1 << bitWidth) - 1)));
    fmt::print("part2: {}\n", part2o2 * part2co2);
}

void Sol::Solve(std::string_view data) {
    return Refactor(data);
}

}